/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ijoja <ijoja@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 00:00:00 by ijoja             #+#    #+#             */
/*   Updated: 2025/02/26 00:00:00 by ijoja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <unistd.h>
#include "minishell.h"

int ms_builtin_echo(char **argv)
{
    int i;
    int newline;

    newline = 1;
    i = 1;
    if (argv[1] && strcmp(argv[1], "-n") == 0)
    {
        newline = 0;
        i = 2;
    }
    while (argv[i])
    {
        if (i > (newline ? 1 : 2))
            write(1, " ", 1);
        write(1, argv[i], strlen(argv[i]));
        i++;
    }
    if (newline)
        write(1, "\n", 1);
    return (0);
}
