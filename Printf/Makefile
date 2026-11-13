NAME	= ft_printf.a

CC		=cc
CFLAGS	= -Wall -Wextra -Werror
AR		= ar rcs
RM	= rm -f

SRCS	= ft_printf.c \
		  ft_printf_putchar_fd.c \
		  ft_printf_putstr_fd.c \
		  ft_printf_putnbr_fd.c \
		  ft_printf_putpoint.c \
		  ft_convertbase.c

OBJS	= $(SRCS:.c=o)

all: $(NAME)

$(NAME): $(OBJS)
	$(AR) $(NAME) $(OBJS)

%.o: %.c ft_printf.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
