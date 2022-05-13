NAME	=	taptap

FLAG	= 	-Wall -Wextra -Werror -g -I ./inc 
DEPS	=	-lncurses
SRCS	=	src/main.c src/utils.c

CC		=	gcc

OBJS	=	$(SRCS:.c=.o)

all		:	$(NAME)

$(NAME)	:	$(OBJS) 
			$(CC) $(FLAG) -o $(NAME) $(OBJS) $(DEPS)

%.o		:	%.c
			$(CC) $(FLAG) -o $@ -c $<  

clean	:
			rm -f $(OBJS) 

fclean	:	clean 
			rm -f $(NAME)

re		:	fclean all