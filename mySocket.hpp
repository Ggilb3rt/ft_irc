#ifndef MY_SOCKET_HPP
    #define MY_SOCKET_HPP

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <iostream>
#include <vector>
#include <map>

#include <queue>

typedef struct	s_lex {
	int			type;
	std::string	lexeme;

}				t_lex;

class user
{

	#define channel std::string
	#define role	bool

	private:
		int			_id;
		std::string	_nick;
		std::string	_name;
		std::map<channel, role>	_channels;

		// user() : _nick(""), _name("") {}

	public:
		user(int fd) :
			_id(fd) {}
		user(int fd, char *nick, char *name) :
			_id(fd),
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

// int user::id_counter = 0;


class mySocket
{
#define ADDRESS_NAME "localhost"
#define BACKLOG 10 // the number of connection allowed on the incomming queue
#define END_MSG "\r\n"
#define MASK (POLLIN + POLLHUP + POLLERR + POLLNVAL)

typedef		std::map<int, user>	user_list;

private:
	struct addrinfo				_hints, *_servinfo, *_p;
	char						*_port;
	int							_master_sockfd, new_fd;
	struct sockaddr_in			their_addr;
	socklen_t					addr_size;
	std::map<int, user>			_users;
	std::vector<struct pollfd>	_pfds;


	void		init();
	void		initAddrInfo();
	void		createMasterSocket();
	int			readData(std::vector<struct pollfd>::iterator);
	void		parse(std::string msg);
	int			handleChange(int	ret_poll, std::vector<struct pollfd>::iterator it);
	void		removeClient(std::vector<struct pollfd>::iterator it);

public:

	mySocket(char *_port);
	~mySocket();

	void	startListen();
	void	printAddrInfo();
};

#endif