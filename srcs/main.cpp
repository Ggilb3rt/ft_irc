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


 void bin(long n)
  {
    long i;
    std::cout << "0";
    for (i = 1 << 30; i > 0; i = i / 2)
    {
      if((n & i) != 0)
      {
        std::cout << "1";
      }
      else
      {
        std::cout << "0";
      }
    }
	std::cout << std::endl;
  }


int main(int argc, char **argv)
{
	if (argc != 3) {
		std::cerr << "usage: ./ft_irc port password" << std::endl;
		return 1;
	}

	(void)argv;
	std::signal(SIGINT, signal_handler);
	ircServer	base(argv[1], argv[2]);
	rplManager	*rpl_manager = rplManager::getInstance();


	/* CHAN_MASK tests */
	int	chan_mode = 0;

	bin(chan_mode);
	chan_mode = set_bit(chan_mode, CHAN_MASK_I);
	bin(chan_mode);
	chan_mode = set_bit(chan_mode, CHAN_MASK_O);
	bin(chan_mode);
	chan_mode = toggle_bit(chan_mode, CHAN_MASK_O);
	bin(chan_mode);
	chan_mode = set_bit(chan_mode, CHAN_MASK_P);
	bin(chan_mode);
	chan_mode = clear_bit(chan_mode, CHAN_MASK_P);
	bin(chan_mode);

	if (get_bit(chan_mode, CHAN_MASK_K))
		std::cout << "Mask K is set\n"; // no
	if (get_bit(chan_mode, CHAN_MASK_I))
		std::cout << "Mask I is set\n"; // yes
	if (get_bit(chan_mode, CHAN_MASK_O))
		std::cout << "Mask O is set\n"; // no
	if (get_bit(chan_mode, CHAN_MASK_P))
		std::cout << "Mask V is set\n"; // no

	bin(chan_mode);
	

	/* Replie manager TESTS */
	std::cout << "rpl_manager addr " << rpl_manager << std::endl;
	// rpl_manager->printAll();
	std::cout << std::endl << rpl_manager->createResponse(RPL_TOPIC, "channelX", "Super topic de ouf");
	// end replie manager tests


	base.printAddrInfo();
	base.startListen();

	delete rpl_manager;

	return 0;
}