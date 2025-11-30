/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env_export.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ijoja <ijoja@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 00:00:00 by ijoja             #+#    #+#             */
/*   Updated: 2025/02/26 00:00:00 by ijoja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "minishell.h"

int ms_builtin_env(t_shell *sh)
{
    size_t  i;

    i = 0;
    while (sh->envp[i])
    {
        printf("%s\n", sh->envp[i]);
        i++;
    }
    return (0);
}

int ms_builtin_export(t_shell *sh, char **argv)
{
    int     status;
    int     i;
    char    *eq;
    char    *key;

    if (!argv[1])
        return (ms_builtin_env(sh));
    status = 0;
    i = 1;
    while (argv[i])
    {
        eq = strchr(argv[i], '=');
        if (eq)
        {
            key = ms_strndup(argv[i], eq - argv[i]);
            if (!key)
                return (1);
            status = ms_env_set(sh, key, eq + 1);
            free(key);
        }
        i++;
    }
    return (status);
}

int ms_builtin_unset(t_shell *sh, char **argv)
{
    int status;
    int i;

    status = 0;
    i = 1;
    while (argv[i])
    {
        status = ms_env_unset(sh, argv[i]);
        i++;
    }
    return (status);
}
