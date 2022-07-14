#include "ircServer.hpp"

void	ircServer::startListen()
{
	int	ret_poll = 0;
	// int	isStuck = 0;
	int quit = 0;
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
			if (_pfds.size() > 2) { // limite arbitraire
				it = _pfds.begin();
				while (ret_poll > 0 && it != end) {
					// std::cout << "iterator when change " << &(*it) << " | " << it->fd << std::endl;
					ret_poll = handleChange(ret_poll, it);
					// std::cout << "iterator after change " << &(*it) << " | " << it->fd << std::endl << std::endl;
					it++;
				}
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
					// if (isStuck)
					// 	continue;
					// removeClient(end);
					// isStuck++;
					// if (isStuck > 5)
						quit = 1;
				}
				else {
					newClient.events = MASK;
					_pfds.push_back(newClient);
					_users.insert(std::pair<int, user>(newClient.fd, user(newClient.fd)));
					it = _pfds.begin();
					end = _pfds.end();
					// UNCOMMENT TO PRINT MAP ON USERS
					// for (user_list::iterator it = _users.begin(); it != _users.end(); it++) {
					// 	std::cout << "id == " << it->second.getId() << std::endl;
				}
			// std::cout << "size " << _pfds.size() << std::endl;
			// }
			// std::cout << "out\n";
			}
			else {
				// std::cout << "nothing appends : " << (_pfds[0].revents & POLLIN) << std::endl;
				it = _pfds.begin();
				while (ret_poll > 0 && it != end) {
					// std::cout << "iterator when change " << &(*it) << " | " << it->fd << std::endl;
					ret_poll = handleChange(ret_poll, it);
					// std::cout << "iterator after change " << &(*it) << " | " << it->fd << std::endl << std::endl;
					it++;
				}
				end = _pfds.end();

				// Answer to client
			}
		}
		// it = _pfds.begin();
		// end = _pfds.end();
		
		// int	new_fd; // original in ircServer class
		// addr_size = sizeof(their_addr);
		// new_fd = accept(_master_sockfd, (struct sockaddr *)&their_addr, &addr_size); // again check // accept will create a new socket to talk with client

		// std::cout << "New client #" << new_fd << " from "
		// 		<< inet_ntoa(their_addr.sin_addr) << ":"
		// 		<< ntohs(their_addr.sin_port) << std::endl;

        // needQuit = this->readData();

		// create user with new_fd
		//std::cout << "User : " << roger.getName() << " fd : " << roger.getFd() << std::endl; 
		// close(new_fd);
	}
}

int		ircServer::handleChange(int	ret_poll, clients_vector::iterator &it) {
	if (it->revents & POLLERR) {
		removeClient(it);
		std::cerr << "error: An error has occured" << std::endl;
	}
	else if (it->revents & POLLHUP) {	// MAC
		std::cerr << "Client " << it->fd << " disconnected" << std::endl;
		removeClient(it);
	}
	// else if (it->revents & POLLRDHUP) { // LINUX
	// 	std::cerr << "Client " << it->fd << " closed connection" << std::endl;
	// 	removeClient(it);
	// }
	else if (it->revents & POLLNVAL) {
		removeClient(it);
		std::cerr << "error: Invalid fd member" << std::endl;
	}
	else if (it->revents & POLLIN) {
		this->readData(it);
		ret_poll--;
	}
	return (ret_poll);
}
