
SRCS =	main.cpp mySocket.cpp

OBJS = ${SRCS:.cpp=.o}

NAME = ft_irc

CFLAGS = -I. -g3 -Wall -Werror -std=c++98
LDFLAGS =

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
	./${NAME} 6697

val_test: all
	valgrind --leak-check=full --track-origins=yes ./${NAME} 6697
