#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>

#include "ircServer.hpp"


int main(int argc, char **argv)
{

	if (argc != 2) {
		std::cerr << "usage: ./ft_irc port password" << std::endl;
		return 1;
	}

	ircServer	base(argv[1]);

	base.printAddrInfo();
	base.startListen();

	return 0;
}