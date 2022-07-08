#include "ircServer.hpp"

void	ircServer::startListen()
{
	int	ret_poll = 0;
	int	isStuck = 0;
	struct pollfd	master;
	
	master.fd = _master_sockfd;
	master.events = POLLIN;


	clients_vector::iterator it = _pfds.begin();
	clients_vector::iterator end = _pfds.end();


	_pfds.push_back(master);

	while (1) { // here must be infinit loop
		ret_poll = poll(_pfds.data(), _pfds.size(), 0);

		if (ret_poll == -1) {
			std::cerr << "ERROR : poll " << errno << " isStuck " << isStuck << std::endl;
		}
		else {
			if (_pfds[0].revents & POLLIN ) {
				struct pollfd	newClient;//!!!!

				addr_size = sizeof(their_addr);
				newClient.fd = accept(_master_sockfd, (struct sockaddr *)&their_addr, &addr_size); 
				if (newClient.fd == -1) {
					// TODO:: Crash Test infinite FD 
						// it works, the program exit() but I don't like it
					std::cerr << "ERROR : accept " << errno << std::endl;
					isStuck++;
					if (isStuck > 5)
						exit(1);
				}
				else {
					newClient.events = MASK;
					_pfds.push_back(newClient);
					_users.insert(std::pair<int, user>(newClient.fd, user(newClient.fd)));
					end = _pfds.end();
					// UNCOMMENT TO PRINT MAP ON USERS
					// for (user_list::iterator it = _users.begin(); it != _users.end(); it++) {
					// 	std::cout << "id == " << it->second.getId() << std::endl;
					// }
				}
			}
			else {
				it = _pfds.begin();
				while (ret_poll > 0 && it != end) {
					ret_poll = handleChange(ret_poll, it);
					it++;
				}
				end = _pfds.end();
				// Answer to client
			}
		}
		

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
	else if (it->revents & POLLHUP) {
		std::cerr << "Client " << it->fd << " disconnected" << std::endl;
		removeClient(it);
	}
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