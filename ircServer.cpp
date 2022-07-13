#include "ircServer.hpp"

/*
	TODO:
	- Finish error management :
		-accept (-1 handled, 0 seems inoperant) but don't want to exit() when -1, just print err and go againt
		-send : must create send()function
		-segfault : none
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


	TODO: DONE
	- Finish error management :
		-listen: (ok)
		-recv : EAGAIN error check on linux
		- error: when buffer uncomplete the program blocks ;
				sending message from another client put it in waiting list
			==> solution : one buffer by client
	-segfault : 
			- scenario :
				- clientX connects, then clientY connects
					clientY disconnects before clientX then clientX disconnects
					(true in branch main)
					==> update _pfds.end() after removeClient()
				- start fd_killer.sh, when process close all sockets, segfault (don't know if problem from fd_killer or us)
					if I close only one socket and use nc => no problems
					==> update iterator in while(ret_poll>0 ...), it was invalidate when removeClient()


*/

ircServer::ircServer(char *port) : _port(port)
{
	this->init();
}

ircServer::~ircServer()
{
	close(_master_sockfd);
	freeaddrinfo(_servinfo);
}
