NAME	=	taptap

FLAG	= 	-Wall -Wextra -Werror -I ./inc -g #-fsanitize=address
DEPS	=	-lncurses
SRCS	=	src/main.c src/terminal.c src/hsignal.c src/utils.c \
			src/split.c src/words.c

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
