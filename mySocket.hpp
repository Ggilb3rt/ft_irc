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
#include <poll.h>
#include <vector>
#include <map>
#include <iostream>

class user
{
	private:
		int			_fd;
		std::string	_name;

	public:
		user() : _fd(-1), _name("") {}
		user(int fd, char *name) : _fd(fd), _name(name) {}
		~user() {}

		std::string	getName() const { return _name; }
		int			getFd() const { return _fd; }
		void		setFd(int fd) { this->_fd = fd; }
		void		setName(char	*name) { this->_name = name; }
};

class mySocket
{

#define ADDRESS_NAME "localhost"
#define BACKLOG 10 // the number of connection allowed on the incomming queue
#define END_MSG "\r\n"

private:
	struct addrinfo		_hints, *_servinfo, *_p;
	char				*_port;
	int					_master_sockfd, new_fd;
	struct sockaddr_in	their_addr;
	socklen_t			addr_size;

	//user				roger;

	void		init();
	void		initAddrInfo();
	void		createMasterSocket();
	int			readData();

public:

	mySocket(char *_port);
	~mySocket();

	void	startListen();
	void	printAddrInfo();
};

#endif