/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd_pwd.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ijoja <ijoja@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 00:00:00 by ijoja             #+#    #+#             */
/*   Updated: 2025/02/26 00:00:00 by ijoja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include "minishell.h"

int ms_builtin_cd(t_shell *sh, char **argv)
{
    const char  *path;
    char        buf[MS_READ_BUFSIZE];

    path = argv[1];
    if (!path)
        path = ms_getenv(sh, "HOME");
    if (!path)
        return (fprintf(stderr, "minishell: cd: HOME not set\n"), 1);
    if (chdir(path) != 0)
        return (perror("cd"), 1);
    if (getcwd(buf, sizeof(buf)))
        ms_env_set(sh, "PWD", buf);
    return (0);
}

int ms_builtin_pwd(void)
{
    char    buf[MS_READ_BUFSIZE];

    if (getcwd(buf, sizeof(buf)))
    {
        printf("%s\n", buf);
        return (0);
    }
    perror("pwd");
    return (1);
}
