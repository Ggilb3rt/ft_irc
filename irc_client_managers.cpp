#include "ircServer.hpp"


void	ircServer::addClient(int fd)
{
	std::pair<users_map::iterator, bool>	ret;

	ret = _users.insert(std::pair<int, user>(fd, user(fd)));
	if (ret.second == false)
		std::cerr << "ERROR : fd " << fd << " is already in use." << std::endl;
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


void	ircServer::removeClient(clients_vector::iterator &it)
{
	channel_map::iterator	chan_it = _channel.begin();
	channel_map::iterator	chan_end = _channel.end();

	while (chan_it != chan_end) {
		chan_it->second.removeUser(it->fd);
		if (chan_it->second.getSize() == 0)
			chan_it = _channel.erase(chan_it);
		else {
			chan_it->second.replaceLastOperator(); // not sure it's needed
			chan_it++;
		}
	}
	close(it->fd);
	_users.erase(it->fd);
	it = _pfds.erase(it);
}