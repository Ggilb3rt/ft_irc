#include "ircServer.hpp"


void	ircServer::addClient(int fd)
{
	std::pair<users_map::iterator, bool>	ret;

	ret = _users.insert(std::pair<int, user>(fd, user(fd)));
	if (ret.second == false) {
		std::cerr << "ERROR : fd " << fd << " is already in use." << std::endl;
		return ;
	}
}

void	ircServer::removeAllUsersFromChans(int id_user)
{
	channel_map::iterator	chan_it = _channel.begin();
	channel_map::iterator	chan_end = _channel.end();

	while (chan_it != chan_end) {
		chan_it->second.removeUser(id_user);
		if (chan_it->second.getSize() == 0) {
			_channel.erase(chan_it);
			chan_it = _channel.begin();
			chan_end = _channel.end();
		}
		else {
			chan_it->second.replaceLastOperator();
			chan_it++;
		}
	}
}

void	ircServer::removeClient(users_map::iterator &it)
{
	removeAllUsersFromChans(it->first);
	close(it->second.getId());
	for (clients_vector::iterator vecit = _pfds.begin(); vecit != _pfds.end(); vecit++) {
		if (vecit->fd == it->first) {
			vecit = _pfds.erase(vecit);
			break ;
		}
	}
	_users.erase(it->first);
}