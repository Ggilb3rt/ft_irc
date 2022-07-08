#include "ircServer.hpp"

void    ircServer::sendToClient(int fd, const char *msg)
{
    std::string res = msg;
    res += END_MSG;
    if (send(fd, res.c_str(), res.length(), 0) == -1) {
        std::cerr << strerror(errno) << std::endl;
        // maybe disconnect client?
    }
}

void	ircServer::removeClient(std::vector<struct pollfd>::iterator &it)
{
	close(it->fd);
	_users.erase(it->fd);
	_pfds.erase(it);
}

// just for debug
void	ircServer::printUsers() {
	for (users_map::iterator i = _users.begin(); i != _users.end(); i++) {
		std::cout << i->first << " : " << i->second.getId() << std::endl;
	}
}

void	ircServer::printAddrInfo()
{
   	char    _ipstr[INET6_ADDRSTRLEN];

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