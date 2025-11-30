/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ijoja <ijoja@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 00:00:00 by ijoja             #+#    #+#             */
/*   Updated: 2025/02/26 00:00:00 by ijoja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdbool.h>
# include <stddef.h>

# define MS_READ_BUFSIZE 4096

typedef struct s_redir
{
    int             type;
    char            *target;
    struct s_redir  *next;
}   t_redir;

typedef struct s_cmd
{
    char            **argv;
    t_redir         *redir;
    struct s_cmd    *next;
}   t_cmd;

typedef struct s_shell
{
    char            **envp;
    int             last_status;
}   t_shell;

int             parse_line(const char *line, t_cmd **out, t_shell *sh);
void            free_commands(t_cmd *cmds);

int             execute_commands(t_shell *sh, t_cmd *cmds);
int             ms_apply_redirs(t_redir *redir);
char            *ms_find_command(const char *cmd, t_shell *sh);
int             ms_run_child(t_shell *sh, t_cmd *cmd, int in_fd, int out_fd);
int             ms_run_single_builtin(t_shell *sh, t_cmd *cmd);

int             is_builtin(const char *cmd);
int             run_builtin(t_shell *sh, char **argv);
int             ms_builtin_echo(char **argv);
int             ms_builtin_cd(t_shell *sh, char **argv);
int             ms_builtin_pwd(void);
int             ms_builtin_env(t_shell *sh);
int             ms_builtin_export(t_shell *sh, char **argv);
int             ms_builtin_unset(t_shell *sh, char **argv);
int             ms_builtin_exit(t_shell *sh, char **argv);

char            *ms_strdup(const char *s);
char            *ms_strndup(const char *s, size_t n);
char            *ms_strjoin_free(char *a, char *b);
char            *ms_itoa(int n);
char            *ms_getenv(t_shell *sh, const char *name);
char            **ms_env_copy(char **envp);
void            ms_env_free(char **env);
int             ms_env_set(t_shell *sh, const char *key, const char *value);
int             ms_env_unset(t_shell *sh, const char *key);
char            *ms_expand_vars(const char *token, t_shell *sh, bool allow);

void            setup_signals_interactive(void);
void            setup_signals_child(void);

#endif
