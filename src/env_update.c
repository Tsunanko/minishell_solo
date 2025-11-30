/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_update.c                                       :+:      :+:    :+:   */
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

static size_t  env_size(char **envp)
{
    size_t  count;

    count = 0;
    while (envp[count])
        count++;
    return (count);
}

static int  env_match(const char *entry, const char *name)
{
    size_t  n;

    n = strlen(name);
    if (strncmp(entry, name, n) == 0 && entry[n] == '=')
        return (1);
    return (0);
}

static char *build_entry(const char *key, const char *value)
{
    size_t  klen;
    size_t  vlen;
    char    *entry;

    klen = strlen(key);
    vlen = value ? strlen(value) : 0;
    entry = malloc(klen + vlen + 2);
    if (!entry)
        return (NULL);
    memcpy(entry, key, klen);
    entry[klen] = '=';
    if (vlen)
        memcpy(entry + klen + 1, value, vlen);
    entry[klen + 1 + vlen] = '\0';
    return (entry);
}

int ms_env_set(t_shell *sh, const char *key, const char *value)
{
    size_t  i;
    size_t  count;
    char    *entry;

    entry = build_entry(key, value);
    if (!entry)
        return (1);
    i = 0;
    while (sh->envp[i])
    {
        if (env_match(sh->envp[i], key))
        {
            free(sh->envp[i]);
            sh->envp[i] = entry;
            return (0);
        }
        i++;
    }
    count = env_size(sh->envp);
    sh->envp = realloc(sh->envp, sizeof(char *) * (count + 2));
    if (!sh->envp)
        return (free(entry), 1);
    sh->envp[count] = entry;
    sh->envp[count + 1] = NULL;
    return (0);
}

int ms_env_unset(t_shell *sh, const char *key)
{
    size_t  count;
    size_t  i;
    size_t  j;
    char    **newenv;

    count = env_size(sh->envp);
    newenv = malloc(sizeof(char *) * count);
    if (!newenv)
        return (1);
    i = 0;
    j = 0;
    while (i < count)
    {
        if (env_match(sh->envp[i], key))
            free(sh->envp[i]);
        else
            newenv[j++] = sh->envp[i];
        i++;
    }
    newenv[j] = NULL;
    free(sh->envp);
    sh->envp = newenv;
    return (0);
}
