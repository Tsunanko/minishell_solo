#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "minishell.h"

extern int g_signal;

static int apply_redirections(t_redir *r)
{
    int fd;
    while (r)
    {
        if (r->type == 0)
        {
            fd = open(r->target, O_RDONLY);
            if (fd < 0 || dup2(fd, STDIN_FILENO) < 0)
                return perror("open"), 1;
            close(fd);
        }
        else if (r->type == 1 || r->type == 2)
        {
            int flags = O_CREAT | O_WRONLY | (r->type == 2 ? O_APPEND : O_TRUNC);
            fd = open(r->target, flags, 0644);
            if (fd < 0 || dup2(fd, STDOUT_FILENO) < 0)
                return perror("open"), 1;
            close(fd);
        }
        else if (r->type == 3)
        {
            int pfd[2];
            if (pipe(pfd) != 0)
                return perror("pipe"), 1;
            char *line = NULL;
            size_t n = 0;
            printf("> ");
            while (getline(&line, &n, stdin) != -1)
            {
                if (strncmp(line, r->target, strlen(r->target)) == 0 && line[strlen(r->target)] == '\n')
                    break;
                write(pfd[1], line, strlen(line));
                printf("> ");
            }
            free(line);
            close(pfd[1]);
            dup2(pfd[0], STDIN_FILENO);
            close(pfd[0]);
        }
        r = r->next;
    }
    return 0;
}

static char *find_command(const char *cmd, t_shell *sh)
{
    if (strchr(cmd, '/'))
        return ms_strdup(cmd);
    char *path = ms_getenv(sh, "PATH");
    if (!path)
        return ms_strdup(cmd);
    char *tmp = ms_strdup(path);
    char *saveptr = NULL;
    for (char *token = strtok_r(tmp, ":", &saveptr); token; token = strtok_r(NULL, ":", &saveptr))
    {
        char buf[4096];
        snprintf(buf, sizeof(buf), "%s/%s", token, cmd);
        if (access(buf, X_OK) == 0)
        {
            char *res = ms_strdup(buf);
            free(tmp);
            return res;
        }
    }
    free(tmp);
    return ms_strdup(cmd);
}

static int child_process(t_shell *sh, t_cmd *cmd, int in_fd, int out_fd)
{
    setup_signals_child();
    if (in_fd != STDIN_FILENO)
    {
        dup2(in_fd, STDIN_FILENO);
        close(in_fd);
    }
    if (out_fd != STDOUT_FILENO)
    {
        dup2(out_fd, STDOUT_FILENO);
        close(out_fd);
    }
    if (apply_redirections(cmd->redir))
        exit(1);
    if (is_builtin(cmd->argv[0]))
        exit(run_builtin(sh, cmd->argv));
    char *path = find_command(cmd->argv[0], sh);
    execve(path, cmd->argv, sh->envp);
    perror(cmd->argv[0]);
    free(path);
    exit(127);
}

static int run_single_builtin(t_shell *sh, t_cmd *cmd)
{
    int saved_in = dup(STDIN_FILENO);
    int saved_out = dup(STDOUT_FILENO);
    int status = 0;
    if (apply_redirections(cmd->redir) == 0)
        status = run_builtin(sh, cmd->argv);
    dup2(saved_in, STDIN_FILENO);
    dup2(saved_out, STDOUT_FILENO);
    close(saved_in);
    close(saved_out);
    return status;
}

int execute_commands(t_shell *sh, t_cmd *cmds)
{
    if (!cmds || !cmds->argv || !cmds->argv[0])
        return 0;
    if (!cmds->next && is_builtin(cmds->argv[0]))
        return run_single_builtin(sh, cmds);
    int status = 0;
    int in_fd = STDIN_FILENO;
    int pipefd[2];
    for (t_cmd *cmd = cmds; cmd; cmd = cmd->next)
    {
        if (cmd->next && pipe(pipefd) < 0)
            return perror("pipe"), 1;
        int out_fd = cmd->next ? pipefd[1] : STDOUT_FILENO;
        pid_t pid = fork();
        if (pid == 0)
            child_process(sh, cmd, in_fd, out_fd);
        if (in_fd != STDIN_FILENO)
            close(in_fd);
        if (cmd->next)
        {
            close(pipefd[1]);
            in_fd = pipefd[0];
        }
    }
    while (wait(&status) > 0)
        ;
    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    return 1;
}

