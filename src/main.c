/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ijoja <ijoja@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 00:00:00 by ijoja             #+#    #+#             */
/*   Updated: 2025/02/26 00:00:00 by ijoja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "minishell.h"

static void set_shlvl(t_shell *sh)
{
    char *lvl = ms_getenv(sh, "SHLVL");
    int num = lvl ? atoi(lvl) : 0;
    char *val = ms_itoa(num + 1);
    if (val)
        ms_env_set(sh, "SHLVL", val);
    free(val);
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
