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

int main(int argc, char **argv)
{

	if (argc != 2) {
		std::cerr << "usage: showip hostname" << std::endl;
		return 1;
	}

	mySocket	base(argv[1]);


	base.printAddrInfo();
	base.startListen();
	//std::cout << "Fd : " << base._master_sockfd << std::endl;

	return 0;
}