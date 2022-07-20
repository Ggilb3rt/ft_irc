#include "ircServer.hpp"

void	ircServer::startListen()
{
	int				ret_poll = 0;
	int 			quit = 0;
	struct pollfd	master;
	
	master.fd = _master_sockfd;
	master.events = POLLIN;


	clients_vector::iterator it = _pfds.begin();
	clients_vector::iterator end = _pfds.end();

	_pfds.push_back(master);

	while (!quit) { // here must be infinit loop
		ret_poll = poll(_pfds.data(), _pfds.size(), 0);

		// std::cout << "How many connections ?: " << _pfds.size() << std::endl;

		if (ret_poll == -1) {
			std::cerr << "ERROR : poll " << errno << std::endl;
		}
		else {
			//! plutot sale mais (en partie) fonctionnel
			if (_pfds.size() > 1020) { // limite arbitraire
				std::cout << "in limite\n";
				it = _pfds.begin();
				while (ret_poll > 0 && it != end) {
					ret_poll = handleChange(ret_poll, it);
					it++;
					// end = _pfds.end();
				}
				it = _pfds.begin();
				end = _pfds.end();
			}
			else if (_pfds[0].revents & POLLIN ) {
				struct pollfd	newClient;//!!!!

				addr_size = sizeof(their_addr);
				std::cout << "poll size " << _pfds.size() << std::endl;
				newClient.fd = accept(_master_sockfd, (struct sockaddr *)&their_addr, &addr_size); 
				if (newClient.fd == -1) {
					// TODO:: Crash Test infinite FD 
						// it works, the program exit() but I don't like it
					std::cerr << "ERROR : accept " << errno << std::endl;
					// quit = 1;
				}
				else {
					newClient.events = MASK;
					_pfds.push_back(newClient);
					this->addClient(newClient.fd);
					it = _pfds.begin();
					end = _pfds.end();
					// this->printUsers();
				}
			}
			else {
				it = _pfds.begin();
				while (ret_poll > 0 && it != end) {
					ret_poll = handleChange(ret_poll, it);
					it++;
					// end = _pfds.end();
				}
				it = _pfds.begin();
				end = _pfds.end();
				// Answer to client
			}
		}
	}
}

int		ircServer::handleChange(int	ret_poll, clients_vector::iterator &it) {
	if (it->revents & POLLERR) {
		removeClient(it);
		std::cerr << "ERROR: An error has occured" << std::endl;
	}
	else if (it->revents & POLLHUP) {	// MAC
		std::cout << "Client " << it->fd << " disconnected" << std::endl;
		removeClient(it);
	}
	else if (it->revents & POLLRDHUP) { // LINUX
		std::cout << "Client " << it->fd << " closed connection" << std::endl;
		removeClient(it);
	}
	else if (it->revents & POLLNVAL) {
		removeClient(it);
		std::cerr << "ERROR: Invalid fd member" << std::endl;
	}
	else if (it->revents & POLLIN) {
		this->readData(it);
		ret_poll--;
	}
	return (ret_poll);
}
