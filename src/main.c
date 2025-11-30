#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "minishell.h"

static void set_shlvl(t_shell *sh)
{
    char *lvl = ms_getenv(sh, "SHLVL");
    int num = lvl ? atoi(lvl) : 0;
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", num + 1);
    setenv("SHLVL", buf, 1);
}

int main(int argc, char **argv, char **envp)
{
    t_shell sh;
    (void)argc;
    (void)argv;
    sh.envp = ms_env_copy(envp);
    sh.last_status = 0;
    set_shlvl(&sh);
    setup_signals_interactive();
    while (1)
    {
        char *line = readline("minishell$ ");
        if (!line)
        {
            printf("exit\n");
            break;
        }
        if (*line)
            add_history(line);
        t_cmd *cmds = NULL;
        if (parse_line(line, &cmds, &sh) == 0)
            sh.last_status = execute_commands(&sh, cmds);
        free_commands(cmds);
        free(line);
    }
    ms_env_free(sh.envp);
    return 0;
}
