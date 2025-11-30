/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_lookup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ijoja <ijoja@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 00:00:00 by ijoja             #+#    #+#             */
/*   Updated: 2025/02/26 00:00:00 by ijoja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include "minishell.h"

static int  env_match(const char *entry, const char *name)
{
    size_t  n;

    n = strlen(name);
    if (strncmp(entry, name, n) == 0 && entry[n] == '=')
        return (1);
    return (0);
}

char    *ms_getenv(t_shell *sh, const char *name)
{
    size_t  i;

    if (!name)
        return (NULL);
    i = 0;
    while (sh->envp[i])
    {
        if (env_match(sh->envp[i], name))
            return (sh->envp[i] + strlen(name) + 1);
        i++;
    }
    return (NULL);
}
