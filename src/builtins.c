#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "minishell.h"

static int ms_export_var(t_shell *sh, const char *arg)
{
    char *eq = strchr(arg, '=');
    if (!eq)
        return 0;
    size_t key_len = eq - arg;
    for (size_t i = 0; sh->envp[i]; i++)
    {
        if (strncmp(sh->envp[i], arg, key_len) == 0 && sh->envp[i][key_len] == '=')
        {
            free(sh->envp[i]);
            sh->envp[i] = ms_strdup(arg);
            return 0;
        }
    }
    size_t count = 0;
    while (sh->envp[count])
        count++;
    char **newenv = calloc(count + 2, sizeof(char *));
    if (!newenv)
        return 1;
    for (size_t i = 0; i < count; i++)
        newenv[i] = sh->envp[i];
    newenv[count] = ms_strdup(arg);
    newenv[count + 1] = NULL;
    free(sh->envp);
    sh->envp = newenv;
    return 0;
}

static int ms_unset_var(t_shell *sh, const char *name)
{
    size_t count = 0;
    while (sh->envp[count])
        count++;
    char **newenv = calloc(count + 1, sizeof(char *));
    if (!newenv)
        return 1;
    size_t j = 0;
    for (size_t i = 0; i < count; i++)
    {
        size_t len = strlen(name);
        if (!(strncmp(sh->envp[i], name, len) == 0 && sh->envp[i][len] == '='))
            newenv[j++] = sh->envp[i];
        else
            free(sh->envp[i]);
    }
    newenv[j] = NULL;
    free(sh->envp);
    sh->envp = newenv;
    return 0;
}

int is_builtin(const char *cmd)
{
    if (!cmd)
        return 0;
    return (!strcmp(cmd, "echo") || !strcmp(cmd, "cd") || !strcmp(cmd, "pwd") ||
            !strcmp(cmd, "export") || !strcmp(cmd, "unset") || !strcmp(cmd, "env") ||
            !strcmp(cmd, "exit"));
}

static int builtin_echo(char **argv)
{
    int i = 1;
    int newline = 1;
    if (argv[1] && strcmp(argv[1], "-n") == 0)
    {
        newline = 0;
        i = 2;
    }
    for (; argv[i]; i++)
    {
        if (i > (newline ? 1 : 2))
            write(1, " ", 1);
        write(1, argv[i], strlen(argv[i]));
    }
    if (newline)
        write(1, "\n", 1);
    return 0;
}

static int builtin_cd(t_shell *sh, char **argv)
{
    const char *path = argv[1] ? argv[1] : ms_getenv(sh, "HOME");
    if (!path)
    {
        fprintf(stderr, "minishell: cd: HOME not set\n");
        return 1;
    }
    if (chdir(path) != 0)
    {
        perror("cd");
        return 1;
    }
    char buf[4096];
    if (getcwd(buf, sizeof(buf)))
    {
        char entry[4100];
        snprintf(entry, sizeof(entry), "PWD=%s", buf);
        ms_export_var(sh, entry);
    }
    return 0;
}

static int builtin_pwd(void)
{
    char buf[4096];
    if (getcwd(buf, sizeof(buf)))
    {
        printf("%s\n", buf);
        return 0;
    }
    perror("pwd");
    return 1;
}

static int builtin_env(t_shell *sh)
{
    for (size_t i = 0; sh->envp[i]; i++)
        printf("%s\n", sh->envp[i]);
    return 0;
}

int run_builtin(t_shell *sh, char **argv)
{
    if (!argv || !argv[0])
        return 0;
    if (strcmp(argv[0], "echo") == 0)
        return builtin_echo(argv);
    if (strcmp(argv[0], "cd") == 0)
        return builtin_cd(sh, argv);
    if (strcmp(argv[0], "pwd") == 0)
        return builtin_pwd();
    if (strcmp(argv[0], "export") == 0)
    {
        if (!argv[1])
            return builtin_env(sh);
        int status = 0;
        for (int i = 1; argv[i]; i++)
            status = ms_export_var(sh, argv[i]);
        return status;
    }
    if (strcmp(argv[0], "unset") == 0)
    {
        int status = 0;
        for (int i = 1; argv[i]; i++)
            status = ms_unset_var(sh, argv[i]);
        return status;
    }
    if (strcmp(argv[0], "env") == 0)
        return builtin_env(sh);
    if (strcmp(argv[0], "exit") == 0)
    {
        int status = argv[1] ? atoi(argv[1]) : 0;
        ms_env_free(sh->envp);
        exit(status);
    }
    return 0;
}

