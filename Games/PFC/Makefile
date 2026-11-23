NAME		= pfc

CC			= cc
CFLAGS		= -Wall -Wextra -Werror
RM			= rm -f

PRINTF_DIR	= ../../Printf
PRINTF		= $(PRINTF_DIR)/libftprintf.a

INCLUDES	= -I . -I $(PRINTF_DIR)

SRCS		= pfc.c
OBJS		= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS) $(PRINTF)
	$(CC) $(CFLAGS) $(OBJS) $(PRINTF) -o $(NAME)

$(PRINTF):
	$(MAKE) -C $(PRINTF_DIR)

%.o: %.c pfc.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJS)
	$(MAKE) -C $(PRINTF_DIR) clean

fclean: clean
	$(RM) $(NAME)
	$(MAKE) -C $(PRINTF_DIR) fclean

re: fclean all

.PHONY: all clean fclean re
