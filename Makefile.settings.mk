# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.settings.mk                               :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lusokol <lusokol@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/21 21:06:11 by rotrojan          #+#    #+#              #
#    Updated: 2022/04/06 16:04:10 by lusokol          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# The name of the binary
NAME = ircserv

# Port to host the server
PORT = 6697

# Password for the server
PSWD = 

# sources needed to compile
SRC =	channelClass.cpp \
		commands.cpp \
		helpers.cpp \
		irc_channel_managers.cpp \
		irc_client_managers.cpp \
		ircServer.cpp \
		listen.cpp \
		main.cpp \
		parse.cpp \
		read.cpp \
		rplManager.cpp \
		setup.cpp \
		bit_mask.cpp

# path of sources
PATH_SRCS = ./srcs

# pathe of includes
INCLUDES =	./includes/

# flags for compilation
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

# compiler used
CXX = c++
