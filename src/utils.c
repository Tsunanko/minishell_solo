#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "minishell.h"

char *ms_strdup(const char *s)
{
    if (!s)
        return NULL;
    size_t len = strlen(s);
    char *out = malloc(len + 1);
    if (!out)
        return NULL;
    memcpy(out, s, len + 1);
    return out;
}

char *ms_strjoin_free(char *a, char *b)
{
    size_t len_a = a ? strlen(a) : 0;
    size_t len_b = b ? strlen(b) : 0;
    char *res = malloc(len_a + len_b + 1);
    if (!res)
        return NULL;
    if (a)
        memcpy(res, a, len_a);
    if (b)
        memcpy(res + len_a, b, len_b);
    res[len_a + len_b] = '\0';
    free(a);
    free(b);
    return res;
}

char **ms_env_copy(char **envp)
{
    size_t count = 0;
    while (envp[count])
        count++;
    char **copy = calloc(count + 1, sizeof(char *));
    if (!copy)
        return NULL;
    for (size_t i = 0; i < count; i++)
        copy[i] = ms_strdup(envp[i]);
    copy[count] = NULL;
    return copy;
}

void ms_env_free(char **env)
{
    if (!env)
        return;
    for (size_t i = 0; env[i]; i++)
        free(env[i]);
    free(env);
}

static int env_keycmp(const char *entry, const char *name)
{
    size_t n = strlen(name);
    if (strncmp(entry, name, n) == 0 && entry[n] == '=')
        return 0;
    return 1;
}

char *ms_getenv(t_shell *sh, const char *name)
{
    if (!name)
        return NULL;
    for (size_t i = 0; sh->envp[i]; i++)
    {
        if (env_keycmp(sh->envp[i], name) == 0)
            return sh->envp[i] + strlen(name) + 1;
    }
    return NULL;
}

