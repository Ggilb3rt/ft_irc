#include "ircServer.hpp"

/*
	TODO:
	- Finish error management :
		-accept (-1 handled, 0 seems inoperant)
		-recv : EAGAIN error check on linux
		-send : must create send()function
		-listen: (ok)
		!- error ?? : when buffer uncomplete the program blocks ;
		!				sending message from another client put it in waiting list
		!===> solution : one buffer by client	
	- Parsing : 
		- check README to get syntax requirements
		- implement lexer
		- ! multi command in single fd
	- Commands :
		- implement commands in README
	- Miscellaneous :
		?- Remove ip v6 ?
		- Code cleanup
			-------------------
		!! Worry about user suppression
		-> multiple instance of the same user can coexist in
			- channels
			- map in socket object
		-> multiple instance of the same channel can coexist in 
			- users
			- socket object
			-------------------
		POSSIBLE SOLUTION
			create channels with user.fd and access clients with fd
*/

ircServer::ircServer(char *port) : _port(port)//, roger()
{
	this->init();
}

ircServer::~ircServer()
{
	close(_master_sockfd);
	freeaddrinfo(_servinfo);
}
