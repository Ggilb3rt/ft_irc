#ifndef IRC_SERVER_HPP
    #define IRC_SERVER_HPP

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
#include "user_class.hpp"
#include "channelClass.hpp"
#include <vector>
#include <map>
#include <queue>

typedef struct	s_lex {
	int			type;
	std::string	lexeme;

}				t_lex;

class ircServer
{
#define ADDRESS_NAME "localhost"
#define BACKLOG 10 // the number of connection allowed on the incomming queue
#define MASK (POLLIN + POLLHUP + POLLERR + POLLNVAL + POLLRDHUP)
#define	MAXLEN_MSG 510

// IRC PROTOCOLE RULES
#define END_MSG "\r\n"
#define MSG_MAX_SIZE 512

typedef		std::map<int, user>				users_map;
typedef		std::vector<struct pollfd>		clients_vector;
typedef		std::map<std::string, channel>	channel_map;

friend		class channel;

private:
	struct addrinfo				_hints, *_servinfo;
	char						*_port;
	int							_master_sockfd;
	struct sockaddr_in			their_addr;
	socklen_t					addr_size;
	users_map					_users;
	clients_vector				_pfds;
	channel_map					_channel;


	// init
	void		init();
	void		initAddrInfo();
	void		createMasterSocket();

	// listen
	int			handleChange(int	ret_poll, std::vector<struct pollfd>::iterator &it);
	void		removeClient(std::vector<struct pollfd>::iterator &it);

	// read and parse
	int			readData(std::vector<struct pollfd>::iterator);
	void		parse(std::string msg);

	// execute
	void		parse(clients_vector::iterator it, std::string query);
	void		handleCommands(clients_vector::iterator it, std::string query);
	void		handleNick(users_map::iterator it, std::string newNick);

	// helpers
	void		sendToClient(int fd, const char *msg);
	void		printUsers();

public:

	ircServer(char *_port);
	~ircServer();

	void	startListen();
	void	printAddrInfo();
};

#endif
