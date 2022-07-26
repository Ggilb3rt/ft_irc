#ifndef IRC_SERVER_HPP
    #define IRC_SERVER_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <cstring>
#include <csignal>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include "irc_protocole_rules.hpp"
#include "rplManager.hpp"
#include "userClass.hpp"
#include "channelClass.hpp"

extern volatile std::sig_atomic_t g_signal_status;

typedef struct	s_lex {
	int			type;
	std::string	lexeme;

}				t_lex;

class ircServer
{
#define ADDRESS_NAME "localhost"
#define BACKLOG 10 // the number of connection allowed on the incomming queue
#define FDLIMIT 1021
#define MASK (POLLIN) // + POLLRDHUP + POLLHUP) //+ POLLERR + POLLNVAL + POLLRDHUP)

#define USER_STATUS_DEL	1


typedef		std::map<int, user>						users_map;
typedef		std::vector<struct pollfd>				clients_vector;
typedef		std::map<std::string, channel>			channel_map;
typedef 	std::pair<channel_map::iterator, bool>	channel_pair;

#define user_id int


friend		class channel;

private:
	struct addrinfo				_hints, *_servinfo;
	char						*_port;
	int							_master_sockfd;
	struct sockaddr_in			their_addr;
	socklen_t					addr_size;


	users_map					_users;
	clients_vector				_pfds;
	unsigned int				_nick_suffixe;
	
	channel_map					_channel;



	// init
	// void			set_signal_status(int signum);
	void			init();
	void			initAddrInfo();
	void			createMasterSocket();

	// listen
	int				handleChange(int	ret_poll, clients_vector::iterator &it);

	// read and parse
	int				readData(clients_vector::iterator);
	void			parse(std::string msg);

	// execute
	void			parse(clients_vector::iterator it, std::string query);
	void			handleCommands(clients_vector::iterator it, std::string query);
	void			handleNick(users_map::iterator it, std::string newNick);


	// client managements
	void			addClient(int fd);
	void			addClient(int fd, std::string nick, std::string name);
	void			removeClient(clients_vector::iterator &it);
	void			removeClient(users_map::iterator &it);

	// channel managements
	void			channelNameCorrection(std::string &name);
	channel_pair	addChannel(std::string name, user_id id);
	void			removeChannel(channel_map::iterator &it);

	// helpers
	void			sendToClient(int fd, const char *msg);
	user_id			getUserByNick(std::string nick);
	users_map::iterator getUserById(user_id id);
	void			printUsers();
	void			printChannels();



	// cmds //? must return char* with response inside
	std::string	topic(user_id id, std::string current_chan, const char *msg = NULL);
	std::string	join(user_id id, std::string chan, std::string key = ""); // key == password ?
	std::string	part(user_id, const std::vector<std::string> chans);
	std::string kick(std::string chan, user_id victim, user_id kicker, std::string comment = "");
	std::string	quit(user_id client, std::string message = "");
	
	std::string	names(std::vector<std::string> chans());
	std::string	list(std::vector<std::string> chans());

public:
	// volatile std::sig_atomic_t	_signal_status;
	
	// void			start_signal();
	// static void		signal_handler(int signum);
	
	ircServer(char *_port);
	~ircServer();

	void	startListen();
	void	printAddrInfo();

};

#endif
