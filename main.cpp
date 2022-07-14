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

	channel		lol("Pouet", 8);
	lol.addUser(1);
	lol.addUser(9);
	lol.addUser(5);
	std::cout << "Channel :\n"
				<< lol.getName() << std::endl
				<< lol.getDescription() << std::endl;
	lol.printUsers();
	lol.sendToAll(base, "lol");

	base.startListen();


	return 0;
}