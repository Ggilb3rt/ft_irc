#include "ircServer.hpp"

void	ircServer::startListen()
{
	int				ret_poll = 0;
	int 			quit = 0;
	struct pollfd	master;
	
	master.fd = _master_sockfd;
	master.events = POLLIN; //? heeeeuuuuu si on avait mis le MASK ici ca n'aurai pas tout regle depuis longtemps ?


	clients_vector::iterator it = _pfds.begin();
	clients_vector::iterator end = _pfds.end();

	_pfds.push_back(master);

	//while (_signal_status != SIGINT) { // here must be infinit loop, quit with signals
	while (!quit) {
		ret_poll = poll(_pfds.data(), _pfds.size(), 0);

		// std::cout << "How many connections ?: " << _pfds.size() << std::endl;

		if (ret_poll == -1) {
			std::cerr << "ERROR : poll " << errno << std::endl;
		}
		else if (ret_poll > 0) {
			if (_pfds[0].revents & POLLIN && _pfds.size() < FDLIMIT) {
				struct pollfd	newClient;//!!!!

				addr_size = sizeof(their_addr);
				newClient.fd = accept(_master_sockfd, (struct sockaddr *)&their_addr, &addr_size); 
				if (newClient.fd == -1) {
					std::cerr << "ERROR : accept " << errno << std::endl;
				}
				else {
					newClient.events = MASK;
					_pfds.push_back(newClient);
					this->addClient(newClient.fd);
					it = _pfds.begin();
					end = _pfds.end();
					this->printUsers();
				}
			}
			// else {
			// 	it = _pfds.begin();
			// 	while (ret_poll > 0 && it != end) {
			// 		std::cout << ret_poll << std::endl;
			// 		ret_poll = handleChange(ret_poll, it);
			// 		it++;
			// 		// end = _pfds.end();
			// 	}
			// 	it = _pfds.begin();
			// 	end = _pfds.end();
			// 	// Answer to client
			// }
			else {
				for (clients_vector::iterator it = _pfds.begin(); it != _pfds.end(); it++) {
					// if ((*it).revents == POLLRDHUP || (*it).revents == POLLHUP) {
					// 	users_map::iterator	user_x = _users.find(it->fd);
					// 	if (user_x != _users.end())
					// 		user_x->second.setStatus(DELETE);
					// }
					if ((*it).revents == POLLIN) {
						this->readData(it);
						ret_poll--;
					}
					if (ret_poll == 0)
						break ;
				}
				for (users_map::iterator it = _users.begin(); it != _users.end(); it++) {
					if (it->second.getStatus() == DELETE) {
						removeClient(it);
						this->printUsers();
						break ;
					}
				}
			}
		}
	}
}

int		ircServer::handleChange(int	ret_poll, clients_vector::iterator &it) {
	// if (it->revents & POLLERR) {
	// 	removeClient(it);
	// 	ret_poll--;
	// 	std::cerr << "ERROR: An error has occured" << std::endl;
	// }
	// else if (it->revents & POLLHUP) {	// MAC
	// 	std::cout << "Client " << it->fd << " disconnected" << std::endl;
	// 	removeClient(it);
	// 	ret_poll--;
	// }
	// else if (it->revents & POLLRDHUP) { // LINUX
	// 	std::cout << "Client " << it->fd << " closed connection" << std::endl;
	// 	removeClient(it);
	// 	ret_poll--;
	// }
	// else if (it->revents & POLLNVAL) {
	// 	removeClient(it);
	// 	ret_poll--;
	// 	std::cerr << "ERROR: Invalid fd member" << std::endl;
	// }
	if (it->revents & POLLIN) {
		std::cout << "read data " << it->fd << std::endl;
		std::cout << "qt of data read : " << this->readData(it) << std::endl;
		ret_poll--;
		std::cout << "retpol and of read " << ret_poll << std::endl;
	}
	return (ret_poll);
}
