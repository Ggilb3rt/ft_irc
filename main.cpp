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
	rplManager	*rpl_manager = rplManager::getInstance();

	/* Replie manager TESTS */
	std::cout << "rpl_manager addr " << rpl_manager << std::endl;
	rpl_manager->printAll();
	std::cout << std::endl << rpl_manager->createResponse(1, "roger");
	std::cout << std::endl << rpl_manager->createResponse(14);
	std::cout << std::endl << rpl_manager->createResponse(RPL_TOPIC, "channelX", "Super topic de ouf");
	// end replie manager tests


	base.printAddrInfo();
	// base.startListen();

	delete rpl_manager;

	return 0;
}