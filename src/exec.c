/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ijoja <ijoja@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 00:00:00 by ijoja             #+#    #+#             */
/*   Updated: 2025/02/26 00:00:00 by ijoja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <readline/readline.h>
#include "minishell.h"

extern int g_signal;

static int open_input(const char *target)
{
    int fd;

    fd = open(target, O_RDONLY);
    if (fd < 0)
    {
        perror(target);
        return (1);
    }
    if (dup2(fd, STDIN_FILENO) < 0)
    {
        perror(target);
        close(fd);
        return (1);
    }
    close(fd);
    return (0);
}

static int open_output(const char *target, int append)
{
    int fd;
    int flags;

    flags = O_CREAT | O_WRONLY;
    if (append)
        flags |= O_APPEND;
    else
        flags |= O_TRUNC;
    fd = open(target, flags, 0644);
    if (fd < 0)
    {
        perror(target);
        return (1);
    }
    if (dup2(fd, STDOUT_FILENO) < 0)
    {
        perror(target);
        close(fd);
        return (1);
    }
    close(fd);
    return (0);
}

static int write_heredoc(const char *delim, int pipefd[2])
{
    char *line;

    while (1)
    {
        line = readline("> ");
        if (!line)
            break ;
        if (strcmp(line, delim) == 0)
        {
            free(line);
            break ;
        }
        write(pipefd[1], line, strlen(line));
        write(pipefd[1], "\n", 1);
        free(line);
    }
    return (0);
}

static int open_heredoc(const char *delim)
{
    int pipefd[2];

    if (pipe(pipefd) != 0)
    {
        perror("pipe");
        return (1);
    }
    write_heredoc(delim, pipefd);
    close(pipefd[1]);
    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[0]);
    return (0);
}

int ms_apply_redirs(t_redir *r)
{
    while (r)
    {
        if (r->type == 0 && open_input(r->target))
            return (1);
        else if (r->type == 1 && open_output(r->target, 0))
            return (1);
        else if (r->type == 2 && open_output(r->target, 1))
            return (1);
        else if (r->type == 3 && open_heredoc(r->target))
            return (1);
        r = r->next;
    }
    return (0);
}

static char *try_path(const char *dir, const char *cmd)
{
    char buf[4096];

    snprintf(buf, sizeof(buf), "%s/%s", dir, cmd);
    if (access(buf, X_OK) == 0)
        return (ms_strdup(buf));
    return (NULL);
}

char *ms_find_command(const char *cmd, t_shell *sh)
{
    char *path;
    char *tmp;
    char *saveptr;
    char *token;
    char *res;

    if (strchr(cmd, '/'))
        return (ms_strdup(cmd));
    path = ms_getenv(sh, "PATH");
    if (!path)
        return (ms_strdup(cmd));
    tmp = ms_strdup(path);
    saveptr = NULL;
    res = NULL;
    token = strtok_r(tmp, ":", &saveptr);
    while (token && !res)
    {
        res = try_path(token, cmd);
        token = strtok_r(NULL, ":", &saveptr);
    }
    if (!res)
        res = ms_strdup(cmd);
    free(tmp);
    return (res);
}

static int setup_io(int in_fd, int out_fd)
{
    if (in_fd != STDIN_FILENO)
    {
        if (dup2(in_fd, STDIN_FILENO) < 0)
            return (1);
        close(in_fd);
    }
    if (out_fd != STDOUT_FILENO)
    {
        if (dup2(out_fd, STDOUT_FILENO) < 0)
            return (1);
        close(out_fd);
    }
    return (0);
}

int ms_run_child(t_shell *sh, t_cmd *cmd, int in_fd, int out_fd)
{
    char *path;

    setup_signals_child();
    if (setup_io(in_fd, out_fd) != 0)
        exit(1);
    if (ms_apply_redirs(cmd->redir) != 0)
        exit(1);
    if (is_builtin(cmd->argv[0]))
        exit(run_builtin(sh, cmd->argv));
    path = ms_find_command(cmd->argv[0], sh);
    execve(path, cmd->argv, sh->envp);
    perror(cmd->argv[0]);
    free(path);
    exit(127);
}

int ms_run_single_builtin(t_shell *sh, t_cmd *cmd)
{
    int saved_in;
    int saved_out;
    int status;

    saved_in = dup(STDIN_FILENO);
    saved_out = dup(STDOUT_FILENO);
    status = 0;
    if (ms_apply_redirs(cmd->redir) == 0)
        status = run_builtin(sh, cmd->argv);
    dup2(saved_in, STDIN_FILENO);
    dup2(saved_out, STDOUT_FILENO);
    close(saved_in);
    close(saved_out);
    return (status);
}

static int wait_children(pid_t last_pid)
{
    int wstatus;
    pid_t wpid;
    int status;

    status = 0;
    while ((wpid = wait(&wstatus)) > 0)
    {
        if (WIFSIGNALED(wstatus) && WTERMSIG(wstatus) == SIGINT)
            write(1, "\n", 1);
        if (wpid == last_pid)
            status = wstatus;
    }
    if (WIFSIGNALED(status))
        return (128 + WTERMSIG(status));
    if (WIFEXITED(status))
        return (WEXITSTATUS(status));
    return (1);
}

int execute_commands(t_shell *sh, t_cmd *cmds)
{
    int in_fd;
    int pipefd[2];
    pid_t last_pid;
    pid_t pid;

    if (!cmds || !cmds->argv || !cmds->argv[0])
        return (0);
    if (!cmds->next && is_builtin(cmds->argv[0]))
        return (ms_run_single_builtin(sh, cmds));
    in_fd = STDIN_FILENO;
    last_pid = -1;
    while (cmds)
    {
        if (cmds->next && pipe(pipefd) < 0)
            return (perror("pipe"), 1);
        pid = fork();
        if (pid == 0)
            ms_run_child(sh, cmds, in_fd, cmds->next ? pipefd[1] : STDOUT_FILENO);
        last_pid = pid;
        if (in_fd != STDIN_FILENO)
            close(in_fd);
        if (cmds->next)
        {
            close(pipefd[1]);
            in_fd = pipefd[0];
        }
        cmds = cmds->next;
    }
    return (wait_children(last_pid));
}
