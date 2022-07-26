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

#include "bit_mask.hpp"
#include "ircServer.hpp"

volatile std::sig_atomic_t g_signal_status = 0;

void	signal_handler(int signal)
{
	g_signal_status = signal;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		std::cerr << "usage: ./ft_irc port password" << std::endl;
		return 1;
	}

	std::signal(SIGINT, signal_handler);
	ircServer	base(argv[1]);
	rplManager	*rpl_manager = rplManager::getInstance();


	/* CHAN_MASK tests */
	int	chan_mode = 0;

	chan_mode = set_bit(chan_mode, CHAN_MASK_M);
	chan_mode = set_bit(chan_mode, CHAN_MASK_I);
	chan_mode = set_bit(chan_mode, CHAN_MASK_O);
	chan_mode = toggle_bit(chan_mode, CHAN_MASK_O);
	chan_mode = clear_bit(chan_mode, CHAN_MASK_V);
	chan_mode = set_bit(chan_mode, CHAN_MASK_V);
	chan_mode = clear_bit(chan_mode, CHAN_MASK_V);

	if (get_bit(chan_mode, CHAN_MASK_B))
		std::cout << "Mask B is set\n"; // no
	if (get_bit(chan_mode, CHAN_MASK_I))
		std::cout << "Mask I is set\n"; // yes
	if (get_bit(chan_mode, CHAN_MASK_M))
		std::cout << "Mask M is set\n"; // yes
	if (get_bit(chan_mode, CHAN_MASK_O))
		std::cout << "Mask O is set\n"; // no
	if (get_bit(chan_mode, CHAN_MASK_V))
		std::cout << "Mask V is set\n"; // no

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