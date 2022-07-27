#include "ircServer.hpp"


void	ircServer::addClient(int fd)
{
	std::pair<users_map::iterator, bool>	ret;

	ret = _users.insert(std::pair<int, user>(fd, user(fd)));
	if (ret.second == false) {
		std::cerr << "ERROR : fd " << fd << " is already in use." << std::endl;
		return ;
	}
	std::cout << "addclient : " << ret.first->second.getId() << " " << fd << std::endl;
}

void		ircServer::addClient(int fd, std::string nick, std::string name)
{
	std::pair<users_map::iterator, bool>	ret;

	if (this->getUserByNick(nick)) {
		std::stringstream	ss;

		ss << _nick_suffixe++;
		nick = "Guest" + ss.str();
		std::cerr << "ERROR : user with same nick, your new nick is " << nick << std::endl;
	}
	ret = _users.insert(std::pair<int, user>(fd, user(fd, nick, name)));
	if (ret.second == false)
		std::cerr << "ERROR : " << fd << " is already in use." << std::endl;
	//! must send a replie to client ?
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
			chan_it->second.replaceLastOperator(); // not sure it's needed
			chan_it++;
		}
	}
}

void	ircServer::removeClient(clients_vector::iterator &it)
{
	removeAllUsersFromChans(it->fd);
	_users.erase(it->fd);
	close(it->fd);
	_pfds.erase(it);
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