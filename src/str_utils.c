/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_utils.c                                        :+:      :+:    :+:   */
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

char    *ms_strdup(const char *s)
{
    size_t  len;
    char    *out;

    if (!s)
        return (NULL);
    len = strlen(s);
    out = malloc(len + 1);
    if (!out)
        return (NULL);
    memcpy(out, s, len + 1);
    return (out);
}

char    *ms_strndup(const char *s, size_t n)
{
    char    *out;

    out = malloc(n + 1);
    if (!out)
        return (NULL);
    memcpy(out, s, n);
    out[n] = '\0';
    return (out);
}

char    *ms_strjoin_free(char *a, char *b)
{
    size_t  len_a;
    size_t  len_b;
    char    *res;

    len_a = a ? strlen(a) : 0;
    len_b = b ? strlen(b) : 0;
    res = malloc(len_a + len_b + 1);
    if (!res)
        return (NULL);
    if (a)
        memcpy(res, a, len_a);
    if (b)
        memcpy(res + len_a, b, len_b);
    res[len_a + len_b] = '\0';
    free(a);
    free(b);
    return (res);
}

char    *ms_itoa(int n)
{
    char            buf[12];
    int             i;
    unsigned int    num;

    i = 11;
    num = (n < 0) ? -n : n;
    buf[i--] = '\0';
    if (num == 0)
        buf[i--] = '0';
    while (num > 0)
    {
        buf[i--] = '0' + (num % 10);
        num /= 10;
    }
    if (n < 0)
        buf[i--] = '-';
    return (ms_strdup(buf + i + 1));
}
