#include "ircServer.hpp"

void	ircServer::sendToClient(int fd, const char *msg)
{
    std::string res = msg;
    res += END_MSG;
    if (send(fd, res.c_str(), res.length(), 0) == -1) {
        std::cerr << strerror(errno) << std::endl;
        // maybe disconnect client?
    }
}


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
			chan_it->second.replaceLastOperator();
			chan_it++;
		}
	}
	close(it->fd);
	_users.erase(it->fd);
	it = _pfds.erase(it);
}

ircServer::channel_pair	ircServer::addChannel(std::string name, user_id id)
{
	channel_pair	ret;

	ret = _channel.insert(std::pair<std::string, channel>(name, channel(name, id)));
	if (ret.second == false)
		std::cerr << "ERROR : channel already exist." << std::endl; //! pas de reponse dans la RFC
	return (ret);
}

void	ircServer::removeChannel(channel_map::iterator &it)
{
	it = _channel.erase(it);
}

user_id	ircServer::getUserByNick(std::string nick)
{
	users_map::iterator		it = _users.begin();
	users_map::iterator		end = _users.end();

	while (it != end) {
		if (it->second.getNick() == nick)
			return (it->first);
		it++;
	}
	return (0);
}

// just for debug
void ircServer::printUsers()
{
	users_map::iterator	it = _users.begin();
	users_map::iterator	end = _users.end();
	
	std::cout << "==All users==\n";
	while (it != end) {
		std::cout << it->first << " ==>\n\t-"
			<< it->second.getId() << "\n\t-"
			<< it->second.getNick() << "\n\t-"
			<< it->second.getName() << "\n";
		it++;
	}
	std::cout << std::endl;
}

void ircServer::printChannels()
{
	channel_map::iterator	it = _channel.begin();
	channel_map::iterator	end = _channel.end();
	
	std::cout << "==All channels==\n";
	while (it != end) {
		std::cout << it->first << " ==>\n\t-"
			<< it->second.getName() << "\n\t-"
			<< it->second.getDescription() << "\n\t-";
			it->second.printUsers();
		it++;
	}
	std::cout << std::endl;
}

void	ircServer::printAddrInfo()
{
	char			_ipstr[INET6_ADDRSTRLEN];
	struct addrinfo	*_p;

	std::cout << std::endl << "IP addresses for " << ADDRESS_NAME << " : " << std::endl << std::endl;
	for(_p = _servinfo;_p != NULL; _p = _p->ai_next) {
		void		*addr;
		std::string	ipver;

		// get the pointer to the address itself,
		// different fields in IPv4 and IPv6:
		if (_p->ai_family == AF_INET) { // IPv4
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)_p->ai_addr;
			std::cout << "Port is " << ntohs(ipv4->sin_port) << std::endl;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		} else { // IPv6
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)_p->ai_addr;
			std::cout << "Port is " << ntohs(ipv6->sin6_port) << std::endl;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		// convert the IP to a string and print it:
		inet_ntop(_p->ai_family, addr, _ipstr, sizeof _ipstr);
		std::cout << "\t" << ipver << " : " << _ipstr << std::endl;
	}
}
