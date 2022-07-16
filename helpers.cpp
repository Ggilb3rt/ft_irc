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

void	ircServer::removeClient(clients_vector::iterator &it)
{
	close(it->fd);
	// must send signal to each of his channels to remove him from list
	_users.erase(it->fd);
	it = _pfds.erase(it);
}

void	ircServer::removeChannel(channel_map::iterator &it)
{
	// must send signal to each of his users to remove him from list
	it = _channel.erase(it);
}

user_id	ircServer::getUserByNick(std::string nick)
{
	// foreach user in users
	// if user->_nick == nick
	//		return user->_id;
	return (-1);
}

// just for debug
void	ircServer::printUsers()
{
	for (users_map::iterator i = _users.begin(); i != _users.end(); i++) {
		std::cout << i->first << " : " << i->second.getId() << std::endl;
	}
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
