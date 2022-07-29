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
					if (it->second.getStatus() == USER_STATUS_DEL) {
						removeClient(it);
						this->printUsers();
						break ;
					}
				}
			}
		}
	}
}
