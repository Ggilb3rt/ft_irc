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

	// channel		lol("Pouet", 8);
	// lol.addUser(1);
	// lol.addUser(5);
	// lol.addUser(9);
	// lol.addUser(5);
	// lol.setDescription(9, "Channel trop nul");
	// lol.setDescription(8, "Fan club de trompettes aux sons incertains");
	// std::cout << "Channel :\n\t"
	// 			<< lol.getName() << std::endl
	// 			<< "\t" << lol.getDescription() << std::endl;
	// lol.printUsers();
	// lol.sendToAll(base, "lol");
	// lol.setUserRole(1, true);
	// lol.setUserRole(8, false);
	// lol.removeUser(9);
	// lol.printUsers();



	// base.startListen();

	delete rpl_manager;

	return 0;
}