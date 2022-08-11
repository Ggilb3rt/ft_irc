#include "ircServer.hpp"

/*
	TODO:
	! Rendre irsServer._host dynamique (cf in pong and setup)
	- VALGRIND errors :
	- Finish error management :
		-segfault :
	- Parsing : 
	- Commands :
	- Miscellaneous :
		?- Remove ip v6 ?
		- Code cleanup
*/

ircServer::ircServer(char *port, std::string pass) : _port(port), _nick_suffixe(0), _pass(pass)
{
	this->init();
}

ircServer::~ircServer()
{
	close(_master_sockfd);
	freeaddrinfo(_servinfo);
}


