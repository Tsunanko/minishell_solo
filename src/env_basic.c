/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_basic.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ijoja <ijoja@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 00:00:00 by ijoja             #+#    #+#             */
/*   Updated: 2025/02/26 00:00:00 by ijoja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include "minishell.h"

char    **ms_env_copy(char **envp)
{
    size_t  count;
    char    **copy;
    size_t  i;

    count = 0;
    while (envp[count])
        count++;
    copy = calloc(count + 1, sizeof(char *));
    if (!copy)
        return (NULL);
    i = 0;
    while (i < count)
    {
        copy[i] = ms_strdup(envp[i]);
        i++;
    }
    copy[count] = NULL;
    return (copy);
}

void    ms_env_free(char **env)
{
    size_t  i;

    if (!env)
        return ;
    i = 0;
    while (env[i])
    {
        free(env[i]);
        i++;
    }
    free(env);
}
