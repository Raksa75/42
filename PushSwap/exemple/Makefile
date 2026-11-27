NAME		= push_swap

CC			= cc
CFLAGS		= -Wall -Wextra -Werror
RM			= rm -f

INCLUDES	= -I includes

SRCS		= srcs/main.c \
			  srcs/parse.c \
			  srcs/parse_numbers.c \
			  srcs/stack.c \
			  srcs/stack_ops.c \
			  srcs/ops.c \
			  srcs/utils.c \
			  srcs/print.c \
			  srcs/bench.c \
			  srcs/sort_small.c \
			  srcs/sort_simple.c \
			  srcs/sort_medium.c \
			  srcs/sort_complex.c \
			  srcs/adaptive.c

OBJS		= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c includes/push_swap.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
