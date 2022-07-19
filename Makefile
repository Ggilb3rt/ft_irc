
SRCS =	main.cpp \
		ircServer.cpp setup.cpp read.cpp listen.cpp \
		irc_client_managers.cpp irc_channel_managers.cpp \
		channelClass.cpp \
		commands.cpp \
		helpers.cpp rplManager.cpp

OBJS = ${SRCS:.cpp=.o}

NAME = ft_irc

CFLAGS = -Wall -Werror -Wextra  -I. -g -std=c++98
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
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./${NAME} 6697

gdb: all
	gdb --args ./${NAME} ${PORT}

fd_killer:
	./fd_killer/fd_killer.sh

