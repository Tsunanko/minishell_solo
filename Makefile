NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror
SRCS = src/main.c \
        src/signals.c \
        src/str_utils.c \
        src/env_basic.c \
        src/env_lookup.c \
        src/env_update.c \
        src/parser.c \
        src/exec.c \
        src/builtins_dispatch.c \
        src/builtin_echo.c \
        src/builtin_cd_pwd.c \
        src/builtin_env_export.c \
        src/builtin_exit.c
OBJS = $(SRCS:.c=.o)
INCLUDES = -Iinclude

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(INCLUDES) -lreadline -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re