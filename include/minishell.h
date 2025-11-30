#ifndef MINISHELL_H
#define MINISHELL_H

#include <stdbool.h>
#include <stddef.h>

typedef struct s_redir
{
    int         type; // 0 in,1 out,2 append,3 heredoc
    char        *target;
    struct s_redir *next;
}   t_redir;

typedef struct s_cmd
{
    char        **argv;
    t_redir     *redir;
    struct s_cmd *next;
}   t_cmd;

typedef struct s_shell
{
    char **envp;
    int last_status;
}   t_shell;

// parser
int     parse_line(const char *line, t_cmd **out, t_shell *sh);
void    free_commands(t_cmd *cmds);

// execution
int     execute_commands(t_shell *sh, t_cmd *cmds);
int     is_builtin(const char *cmd);
int     run_builtin(t_shell *sh, char **argv);

// utils
char    *ms_strdup(const char *s);
char    *ms_strjoin_free(char *a, char *b);
char    *ms_getenv(t_shell *sh, const char *name);
char    **ms_env_copy(char **envp);
void    ms_env_free(char **env);
char    **ms_split_keep_quotes(const char *line);
char    *ms_expand_vars(const char *token, t_shell *sh, bool in_double_quotes);

// signals
void    setup_signals_interactive(void);
void    setup_signals_child(void);

#endif
