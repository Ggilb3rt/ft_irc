
SRCS =	main.cpp ircServer.cpp setup.cpp read.cpp listen.cpp helpers.cpp commands.cpp

OBJS = ${SRCS:.cpp=.o}

NAME = ft_irc

CFLAGS = -I. -g3 -Wall -Werror -std=c++98
LDFLAGS =

CC = c++
RM = rm -f

PORT = 6697
HOST = 127.0.0.1

${NAME}:	${OBJS}
		${CC} -o ${NAME} ${OBJS} ${LDFLAGS}

all:		${NAME}

clean:
		${RM} ${OBJS} *~ #*#

fclean:		clean
		${RM} ${NAME}

re:		fclean all

test: all
	./${NAME} ${PORT}

val_test: all
	valgrind --leak-check=full --track-origins=yes ./${NAME} 6697

ns_test:
	./nc_test.sh