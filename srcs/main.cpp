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

volatile std::sig_atomic_t g_signal_status = 0;

void	signal_handler(int signal)
{
	g_signal_status = signal;
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		std::cerr << "usage: ./ft_irc port password" << std::endl;
		return 1;
	}

	std::signal(SIGINT, signal_handler);
	ircServer	base(argv[1], argv[2]);
	rplManager	*rpl_manager = rplManager::getInstance();

	base.printAddrInfo();
	base.startListen();

	delete rpl_manager;
	return 0;
}