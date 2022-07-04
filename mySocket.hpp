#ifndef MY_SOCKET_HPP
    #define MY_SOCKET_HPP

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>


class mySocket
{

#define ADDRESS_NAME "localhost"
#define BACKLOG 10 // the number of connection allowed on the incomming queue

private:
	struct addrinfo		_hints, *_servinfo, *_p;
	char				*_port;
	int					sockfd, new_fd;
	struct sockaddr_in	their_addr;
	socklen_t			addr_size;


	void		initAddrInfo();
	void		createSocketFd();
	int			readData();

public:

	mySocket(char *_port);
	~mySocket();

	void	startListen();
	void	printAddrInfo();
};

#endif