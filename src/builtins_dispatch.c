/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_dispatch.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ijoja <ijoja@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 00:00:00 by ijoja             #+#    #+#             */
/*   Updated: 2025/02/26 00:00:00 by ijoja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include "minishell.h"

int is_builtin(const char *cmd)
{
    if (!cmd)
        return (0);
    if (!strcmp(cmd, "echo") || !strcmp(cmd, "cd") || !strcmp(cmd, "pwd"))
        return (1);
    if (!strcmp(cmd, "export") || !strcmp(cmd, "unset") || !strcmp(cmd, "env"))
        return (1);
    if (!strcmp(cmd, "exit"))
        return (1);
    return (0);
}

int run_builtin(t_shell *sh, char **argv)
{
    if (!argv || !argv[0])
        return (0);
    if (!strcmp(argv[0], "echo"))
        return (ms_builtin_echo(argv));
    if (!strcmp(argv[0], "cd"))
        return (ms_builtin_cd(sh, argv));
    if (!strcmp(argv[0], "pwd"))
        return (ms_builtin_pwd());
    if (!strcmp(argv[0], "export"))
        return (ms_builtin_export(sh, argv));
    if (!strcmp(argv[0], "unset"))
        return (ms_builtin_unset(sh, argv));
    if (!strcmp(argv[0], "env"))
        return (ms_builtin_env(sh));
    if (!strcmp(argv[0], "exit"))
        return (ms_builtin_exit(sh, argv));
    return (0);
}
