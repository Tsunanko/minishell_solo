NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror
SRCS = src/main.c src/utils.c src/parser.c src/exec.c src/builtins.c src/signals.c
OBJS = $(SRCS:.c=.o)
INCLUDES = -Iinclude

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(INCLUDES) -lreadline -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
