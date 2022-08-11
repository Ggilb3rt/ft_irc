#include "ircServer.hpp"

ircServer::ircServer(char *port, std::string pass) : _port(port), _nick_suffixe(0), _pass(pass)
{
	this->init();
}

ircServer::~ircServer()
{
	close(_master_sockfd);
	freeaddrinfo(_servinfo);
}
