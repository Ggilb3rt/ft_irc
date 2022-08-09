#include "ircServer.hpp"

void	ircServer::startListen()
{
	int				ret_poll = 0;
	struct pollfd	master;

	master.fd = _master_sockfd;
	master.events = POLLIN;
	_pfds.push_back(master);
	while (g_signal_status != SIGINT) {
		ret_poll = poll(_pfds.data(), _pfds.size(), 0);
		if (ret_poll == -1) {
			std::cerr << "ERROR : poll " << errno << std::endl;
		}
		else if (ret_poll > 0) {
			if (_pfds[0].revents & POLLIN && _pfds.size() < FDLIMIT) {
				struct pollfd	newClient;	//!!!!

				addr_size = sizeof(their_addr);
				newClient.fd = accept(_master_sockfd, (struct sockaddr *)&their_addr, &addr_size); 
				if (newClient.fd == -1) {
					std::cerr << "ERROR : accept " << errno << std::endl;
				}
				else {
					newClient.events = MASK;
					_pfds.push_back(newClient);
					this->addClient(newClient.fd);
					this->printUsers();
				}
			}
			else {
				for (clients_vector::iterator it = _pfds.begin(); it != _pfds.end(); it++) {
					// if ((*it).revents == POLLRDHUP || (*it).revents == POLLHUP) {
					// 	users_map::iterator	user_x = _users.find(it->fd);
					// 	if (user_x != _users.end())
					// 		user_x->second.setStatus(USER_STATUS_DEL);
					// }
					if ((*it).revents == POLLIN) {
						this->readData(it);
						ret_poll--;
					}
					if (ret_poll == 0)
						break ;
				}
				for (users_map::iterator it = _users.begin(); it != _users.end(); it++) {
					std::cout << "STATUS[" << it->second.getNick() << "] == " << it->second.getStatus() << std::endl;
					if (it->second.getStatus() == USER_STATUS_DEL) {
						removeClient(it);
						this->printUsers();
						break ;
					}
					else if (it->second.getStatus() == USER_STATUS_PENDING) {
						registerUser(it);
						//check client buffer for CAP USER and NICK and 
					}
				}
			}
		}
	}
}

std::string	getChunk(std::string msg, std::string param) {
	std::string chunk;
	size_t pos_param;
	size_t pos_end;

	pos_param = msg.find(param);
	pos_end = msg.find(END_MSG, pos_param);
	if (pos_param == std::string::npos || pos_end == std::string::npos) {
		return (std::string());
	}
	// get from param to end_msg
	chunk = msg.substr(pos_param, pos_end - pos_param);
	return (chunk);
}

void	ircServer::registerUser(users_map::iterator &it) {
	if (getChunk(it->second._msg, "CAP").size() > 0 && 
		getChunk(it->second._msg, "PASS").size() > 0  &&
		getChunk(it->second._msg, "NICK").size() > 0  &&
		getChunk(it->second._msg, "USER").size() > 0  ) {
		if (!parse(it, getChunk(it->second._msg, "PASS"))) {
			it->second.setStatus(USER_STATUS_DEL);
			return ;
		}
		if (!parse(it, getChunk(it->second._msg, "NICK"))) {
			it->second.setStatus(USER_STATUS_DEL);
			return ;
		}
		if (!parse(it, getChunk(it->second._msg, "USER"))) {
			std::cout << "sil te plait ne t'affiche pas\n";
			it->second.setStatus(USER_STATUS_DEL);
			return ;
		}
		it->second.setStatus(USER_STATUS_CONNECTED);
		it->second._msg.clear();
		sendToClient(it->first, RPL_OKCONN);
		// sendToClient(it->first, RPL_OKCONN, it->second.getNick());
	}
	if (getChunk(it->second._msg, "USER").size() > 0  ) {
		// send password required;
		std::cout << "pas de pass\n";
		return ;
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
