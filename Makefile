
SRCS =	main.cpp

OBJS = ${SRCS:.cpp=.o}

NAME = ft_irc

CFLAGS = -I. -g3 -Wall -Werror -std=c++98
LDFLAGS = -g

CC = c++
RM = rm -f

${NAME}:	${OBJS}
		${CC} -o ${NAME} ${OBJS} ${LDFLAGS}

all:		${NAME}

clean:
		${RM} ${OBJS} *~ #*#

fclean:		clean
		${RM} ${NAME}

re:		fclean all

test: all
	./${NAME} localhost
	@echo "\n\n"
	#./${NAME} google.com
	
