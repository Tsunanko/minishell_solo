/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ijoja <ijoja@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 00:00:00 by ijoja             #+#    #+#             */
/*   Updated: 2025/02/26 00:00:00 by ijoja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "minishell.h"

static char *read_word(const char *line, size_t *i, bool *expand)
{
    size_t start = *i;
    char quote = 0;
    *expand = true;
    char *res = ms_strdup("");
    while (line[*i])
    {
        char c = line[*i];
        if (!quote && (c == '\'' || c == '"'))
        {
            quote = c;
            if (quote == '\'')
                *expand = false;
            (*i)++;
            continue;
        }
        if (quote && c == quote)
        {
            quote = 0;
            (*i)++;
            continue;
        }
        if (!quote && (isspace((unsigned char)c) || c == '|' || c == '<' || c == '>'))
            break;
        char str[2] = {c, 0};
        res = ms_strjoin_free(res, ms_strdup(str));
        (*i)++;
    }
    if (quote)
    {
        fprintf(stderr, "minishell: unmatched quote\n");
        free(res);
        return NULL;
    }
    if (*i == start)
        return NULL;
    return res;
}

static int append_token(char ***tokens, bool **expand, size_t *count, char *tok, bool exp)
{
    char **new_tokens = realloc(*tokens, sizeof(char *) * (*count + 2));
    bool *new_expand = realloc(*expand, sizeof(bool) * (*count + 2));
    if (!new_tokens || !new_expand)
        return 1;
    *tokens = new_tokens;
    *expand = new_expand;
    (*tokens)[*count] = tok;
    (*expand)[*count] = exp;
    (*count)++;
    (*tokens)[*count] = NULL;
    return 0;
}

static int tokenize(const char *line, char ***tokens, bool **expand)
{
    size_t i = 0, count = 0;
    *tokens = NULL;
    *expand = NULL;
    while (line[i])
    {
        while (isspace((unsigned char)line[i]))
            i++;
        if (!line[i])
            break;
        if (line[i] == '|' || line[i] == '<' || line[i] == '>')
        {
            char buf[3] = {0};
            buf[0] = line[i];
            if ((line[i] == '<' || line[i] == '>') && line[i + 1] == line[i])
            {
                buf[1] = line[i + 1];
                i += 2;
            }
            else
                i++;
            if (append_token(tokens, expand, &count, ms_strdup(buf), true))
                return 1;
            continue;
        }
        bool exp = true;
        char *word = read_word(line, &i, &exp);
        if (!word)
        {
            for (size_t j = 0; j < count; j++)
                free((*tokens)[j]);
            free(*tokens);
            free(*expand);
            *tokens = NULL;
            *expand = NULL;
            return 1;
        }
        if (append_token(tokens, expand, &count, word, exp))
            return 1;
    }
    return 0;
}

static char *expand_value(const char *name, t_shell *sh)
{
    if (strcmp(name, "?") == 0)
    {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", sh->last_status);
        return ms_strdup(buf);
    }
    char *val = ms_getenv(sh, name);
    return val ? ms_strdup(val) : ms_strdup("");
}

char *ms_expand_vars(const char *token, t_shell *sh, bool allow)
{
    if (!allow)
        return ms_strdup(token);
    char *res = ms_strdup("");
    for (size_t i = 0; token[i]; i++)
    {
        if (token[i] == '$' && (isalpha((unsigned char)token[i + 1]) || token[i + 1] == '_' || token[i + 1] == '?'))
        {
            size_t start = i + 1;
            i++;
            while (token[i] && (isalnum((unsigned char)token[i]) || token[i] == '_') && token[start] != '?')
                i++;
            char *name = ms_strndup(token + start, i - start);
            char *val = expand_value(name, sh);
            res = ms_strjoin_free(res, val);
            free(name);
            i--;
        }
        else
        {
            char buf[2] = {token[i], 0};
            res = ms_strjoin_free(res, ms_strdup(buf));
        }
    }
    return res;
}

static t_redir *redir_new(int type, char *target)
{
    t_redir *r = calloc(1, sizeof(t_redir));
    if (!r)
        return NULL;
    r->type = type;
    r->target = target;
    r->next = NULL;
    return r;
}

static int add_redir(t_cmd *cmd, int type, char *target)
{
    t_redir *new = redir_new(type, target);
    if (!new)
        return 1;
    if (!cmd->redir)
        cmd->redir = new;
    else
    {
        t_redir *tmp = cmd->redir;
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = new;
    }
    return 0;
}

static int build_commands(char **tokens, bool *exp_flags, t_cmd **out, t_shell *sh)
{
    t_cmd *head = NULL, *tail = NULL;
    t_cmd *cur = calloc(1, sizeof(t_cmd));
    if (!cur)
        return 1;
    head = tail = cur;
    size_t argv_cap = 4, argv_len = 0;
    cur->argv = calloc(argv_cap, sizeof(char *));
    if (!cur->argv)
        return 1;
    for (size_t i = 0; tokens[i]; i++)
    {
        char *tok = tokens[i];
        if (strcmp(tok, "|") == 0)
        {
            cur->argv[argv_len] = NULL;
            cur = calloc(1, sizeof(t_cmd));
            if (!cur)
                return 1;
            cur->argv = calloc(4, sizeof(char *));
            argv_cap = 4;
            argv_len = 0;
            tail->next = cur;
            tail = cur;
            continue;
        }
        int type = -1;
        if (strcmp(tok, "<") == 0) type = 0;
        else if (strcmp(tok, ">") == 0) type = 1;
        else if (strcmp(tok, ">>") == 0) type = 2;
        else if (strcmp(tok, "<<") == 0) type = 3;
        if (type != -1)
        {
            if (!tokens[i + 1])
            {
                fprintf(stderr, "minishell: missing redirection target\n");
                free(cur->argv);
                return 1;
            }
            i++;
            char *target = ms_expand_vars(tokens[i], sh, exp_flags[i]);
            if (add_redir(cur, type, target))
                return 1;
            continue;
        }
        if (argv_len + 1 >= argv_cap)
        {
            argv_cap *= 2;
            char **newv = realloc(cur->argv, sizeof(char *) * argv_cap);
            if (!newv)
                return 1;
            cur->argv = newv;
        }
        cur->argv[argv_len++] = ms_expand_vars(tok, sh, exp_flags[i]);
    }
    cur->argv[argv_len] = NULL;
    *out = head;
    return 0;
}

int parse_line(const char *line, t_cmd **out, t_shell *sh)
{
    char **tokens = NULL;
    bool *exp_flags = NULL;
    if (tokenize(line, &tokens, &exp_flags))
        return 1;
    if (!tokens || !tokens[0])
    {
        *out = NULL;
        free(tokens);
        free(exp_flags);
        return 0;
    }
    int res = build_commands(tokens, exp_flags, out, sh);
    for (size_t i = 0; tokens && tokens[i]; i++)
        free(tokens[i]);
    free(tokens);
    free(exp_flags);
    return res;
}

void free_commands(t_cmd *cmds)
{
    while (cmds)
    {
        t_cmd *next = cmds->next;
        if (cmds->argv)
        {
            for (size_t i = 0; cmds->argv[i]; i++)
                free(cmds->argv[i]);
            free(cmds->argv);
        }
        t_redir *r = cmds->redir;
        while (r)
        {
            t_redir *rn = r->next;
            free(r->target);
            free(r);
            r = rn;
        }
        free(cmds);
        cmds = next;
    }
}

