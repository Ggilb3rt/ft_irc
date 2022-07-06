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

	#define channel std::string
	#define role	bool

	private:
		int			_id;
		std::string	_nick;
		std::string	_name;
		std::map<channel, role>	_channels;

		user() : _nick(""), _name("") {}
		static int	id_counter;

	public:
		user(char *nick, char *name) :
			_id(id_counter++),
			_nick(nick),
			_name(name),
			_channels() {}
		~user() {}

		int			getId() const {return _id;}
		std::string	getNick() const { return _nick; }
		void		setNick(char	*nick) { /*check if new nick exist*/this->_nick = nick; }
		std::string	getName() const { return _name; }
		void		setName(char	*name) { this->_name = name; }
};


int user::id_counter = 0;

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