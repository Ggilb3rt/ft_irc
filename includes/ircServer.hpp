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

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include "irc_protocole_rules.hpp"
#include "bit_mask.hpp"
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

#define USER_STATUS_DEL	2
#define USER_STATUS_PENDING 0
#define USER_STATUS_CONNECTED 1


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

	std::string					_pass;
	std::string					_host;

	// init
	void			init();
	void			initAddrInfo();
	void			createMasterSocket();

	// listen
	int				handleChange(int	ret_poll, clients_vector::iterator &it);

	// read and parse
	int				readData(clients_vector::iterator);
	bool			parse(users_map::iterator &it, std::string query);
	bool    		handleCommands(users_map::iterator &it, std::vector<std::string> &argvec);


	// client managements
	void			addClient(int fd);
	void			addClient(int fd, std::string nick, std::string name);
	void			registerUser(users_map::iterator &it);
	void			removeAllUsersFromChans(int id_user);
	void			removeClient(users_map::iterator &it);

	// channel managements
	void			channelNameCorrection(std::string &name);
	channel_pair	addChannel(std::string name, user_id id);
	void			removeChannel(channel_map::iterator &it);

	// helpers
	std::string		toLower(std::string nickname);
	bool			isAlnum(char c);
	bool			isSpecialOk(char c);
	bool			isValid(std::string &nickname);
	void			sendToClient(int fd, int code, std::string param_1 = "", std::string param_2 = "");
	void    		sendToClient(int fd_sender, int fd_reciver, int code, std::string param_1 = "", std::string param_2 = "");
	void			sendToChannel(user sender, channel chan, int code, std::string before = "", std::string after = "");
	user_id			getUserByNick(std::string nick) const;
	users_map::iterator getUserById(user_id id);
	void			namesRplConditions(users_map::iterator &user,
								channel_map::iterator &all_chans_it);
	void			listRplConditions(users_map::iterator &user,
								channel_map::iterator &all_chans_it,
								rplManager *rpl_manager);
	void			printUsers();
	void			printChannels();



	// cmds //! il faudrait que tout les params soient const
	bool	handleUser(users_map::iterator user, std::vector<std::string> &argvec);
	bool	privmsg(users_map::iterator pair, std::vector<std::string> &argvec);
	bool	notice(users_map::iterator pair, std::vector<std::string> &argvec);
	bool	nick(users_map::iterator user, std::vector<std::string> &argvec);
	bool	pass(users_map::iterator user, std::vector<std::string> &argvec);
	bool	topic(users_map::iterator user, std::vector<std::string> params);
	bool	join(users_map::iterator user, std::vector<std::string> params);
	bool	part(users_map::iterator user, const std::vector<std::string> params);
	bool	kick(users_map::iterator user, const std::vector<std::string> params);
	bool	quit(users_map::iterator user, std::vector<std::string> params);
	bool	mode(users_map::iterator user, std::vector<std::string> params);
	bool	names(users_map::iterator user, std::vector<std::string> params);
	bool	list(users_map::iterator user, std::vector<std::string> params);
	bool	invite(users_map::iterator user, std::vector<std::string> params);
	
	bool	pong(users_map::iterator user, std::vector<std::string> params);


public:
	ircServer(char *_port, std::string pass);
	~ircServer();

	void	startListen();
	void	printAddrInfo();

};

#endif
