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

#include "mySocket.hpp"

int user::id_counter = 0;


int main(int argc, char **argv)
{

	if (argc != 2) {
		std::cerr << "usage: ./ft_irc port password" << std::endl;
		return 1;
	}

	user		poeut((char *)"roger", (char *)"roger");
	mySocket	base(argv[1]);

	base.printAddrInfo();
	base.startListen();

	return 0;
}