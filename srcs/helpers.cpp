#include "ircServer.hpp"

void    ircServer::sendToClient(int fd_sender, int fd_reciver, int code, std::string param_1, std::string param_2)
{
    rplManager    *instance = rplManager::getInstance();
    std::string    res    = instance->createResponse(getUserById(fd_sender)->second, code, param_1, param_2);
    std::cout << "response == " << res << std::endl;
    if (send(fd_reciver, res.c_str(), res.length(), 0) == -1) {
        std::cerr << strerror(errno) << std::endl;
    }
}

void	ircServer::sendToClient(int fd, int code, std::string param_1, std::string param_2)
{
	rplManager	*instance = rplManager::getInstance();
	std::string	res	= instance->createResponse(getUserById(fd)->second, code, param_1, param_2);
	std::cout << "response == " << res << std::endl;
    if (send(fd, res.c_str(), res.length(), 0) == -1) {
        std::cerr << strerror(errno) << std::endl;
    }
}

user_id	ircServer::getUserByNick(const std::string nick) const
{
	for (users_map::const_iterator it_user = _users.begin();
		it_user != _users.end();
		it_user++) {
			if (it_user->second.getNick().compare(nick.c_str()) == 0)
				return (it_user->first);
	}
	return (0);
}

ircServer::users_map::iterator	ircServer::getUserById(user_id id)
{
	return (_users.find(id));
}

void	ircServer::namesRplConditions(users_map::iterator &user, channel_map::iterator &all_chans_it)
{
	users_map::iterator			current_user;
	std::string					res;

	if (all_chans_it->second.isFlagSets(CHAN_MASK_P)
		|| all_chans_it->second.isFlagSets(CHAN_MASK_S)) {
		if (!all_chans_it->second.isOnChannel(user->first))
			return ;
	}
	if (all_chans_it->second.isFlagSets(CHAN_MASK_P))
		res = "* ";
	else if (all_chans_it->second.isFlagSets(CHAN_MASK_S))
		res = "@ ";
	else
		res = "= ";
	res += all_chans_it->first;
	res += " :";
	for (channel::users_list_const_it user_in_chan = all_chans_it->second._users.begin();
		user_in_chan != all_chans_it->second._users.end(); user_in_chan++) {
		current_user = _users.find(user_in_chan->first);
		if (current_user != _users.end()) {
			if (user_in_chan->second)
				res += "@";
			res += current_user->second.getNick();
			res += " ";
		}
	}
	sendToClient(user->first, RPL_NAMREPLY, res);
	sendToClient(user->first, RPL_ENDOFNAMES, all_chans_it->first);
}

void	ircServer::listRplConditions(users_map::iterator &user, channel_map::iterator &all_chans_it, rplManager *rpl_manager)
{
	(void)rpl_manager;
	std::string			rep(all_chans_it->first);
	std::stringstream	ss;

	rep += " ";
	ss << all_chans_it->second._users.size();
	rep += ss.str();
	if (all_chans_it->second.isFlagSets(CHAN_MASK_S)) {
		if (all_chans_it->second.isOnChannel(user->first))
			sendToClient(user->first, RPL_LIST, rep, all_chans_it->second.getDescription());
	}
	else if (all_chans_it->second.isFlagSets(CHAN_MASK_P)) {
		if (all_chans_it->second.isOnChannel(user->first))
			sendToClient(user->first, RPL_LIST, rep, all_chans_it->second.getDescription());
		else
			sendToClient(user->first, RPL_LIST, rep, "Prv");
	}
	else {
		sendToClient(user->first, RPL_LIST, rep, all_chans_it->second.getDescription());
	}
}


void ircServer::printUsers()
{
	users_map::iterator	it = _users.begin();
	users_map::iterator	end = _users.end();
	
	std::cout << "==All users==\n";
	while (it != end) {
		std::cout << it->first << " ==>\n\tid : "
			<< it->second.getId() << "\n\tnick : "
			<< it->second.getNick() << "\n\tname : "
			<< it->second.getName() << "\n\tstatus : "
			<< it->second.getStatus() << "\n";
		it++;
	}
	std::cout << std::endl;
}

void ircServer::printChannels()
{
	channel_map::iterator	it = _channel.begin();
	channel_map::iterator	end = _channel.end();
	
	std::cout << "==All channels==\n";
	while (it != end) {
		std::cout << it->first << " ==>\n\t- Name : "
			<< it->second.getName() << "\n\t- Desc : "
			<< it->second.getDescription() << "\n\t- Modes : "
			<< it->second.convertModeMaskToFlags() << "\n\t-";
			it->second.printUsers();
		it++;
	}
	std::cout << std::endl;
}

void	ircServer::printAddrInfo()
{
	struct addrinfo	*_p;

	std::cout << std::endl << "Listening on : " << ADDRESS_NAME << std::endl << std::endl;
	for(_p = _servinfo;_p != NULL; _p = _p->ai_next) {

		if (_p->ai_family == AF_INET) {
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)_p->ai_addr;
			std::cout << "Port is " << ntohs(ipv4->sin_port) << std::endl;
		}

	}
}

std::string	ircServer::toLower(std::string nickname) {

	std::string	lower;

	int	i = 0;

	while (nickname.c_str()[i]) {
		if (nickname.c_str()[i] >= 'A' && nickname.c_str()[i] <= 'Z')
			lower += nickname.c_str()[i] + 32;
		else
			lower += nickname.c_str()[i];
		i++;
	}
	return (lower);
}

bool	ircServer::isAlnum(char c) {
	if (c >= '0' && c <= '9')
		return (true);
	if (c >= 'a' && c <= 'z')
		return (true);
	if (c >= 'A' && c <= 'Z')
		return (true);
	return (false);
}

bool	ircServer::isSpecialOk(char c) {
	if ((c >= '[' && c <= '}') || c == '-')
		return (true);
	return (false);
}

bool	ircServer::isValid(std::string &nickname) {
	int	i = 0;

	if (nickname.c_str()[i] >= '0' && nickname.c_str()[i] <= '9')
		return (false);
	while (nickname.c_str()[i]) {
		if (!isAlnum(nickname.c_str()[i]))
			return (false);
		if (!isSpecialOk(nickname.c_str()[i]))
			return (false);
		if (nickname.c_str()[i] == ' ')
			nickname[i] = '_';
		i++;
	}
	return (true);
}

void	ircServer::sendToChannel(user sender, channel chan, int code, std::string before, std::string after) {
	channel::users_list::iterator pos;
	channel::users_list::iterator end = chan.getEnd();
	int				sender_id = sender.getId();

	pos = chan.getUsers();
	while (pos != end) {
		if (pos->first != sender_id)
			sendToClient(sender.getId(), pos->first, code, before, after);
		pos++;
	}
}

bool			ircServer::modeRemoveUserOperator(int &modes_to_remove, std::vector<std::string> &params,
											users_map::iterator &user, channel_map::iterator it_chan)
{
	if (get_bit(modes_to_remove, CHAN_MASK_O)) {
		if (params.size() != 3) {
			std::string res(it_chan->first + " MODE");
			sendToClient(user->first, ERR_NEEDMOREPARAMS, res);
			return (false);
		}
		if (user->first != this->getUserByNick(params[2])) {
				sendToClient(user->first, ERR_USERSDONTMATCH);
				return (false);
		}
		it_chan->second.setUserRole(this->getUserByNick(user->second.getNick()), false);
		modes_to_remove = clear_bit(modes_to_remove, CHAN_MASK_O);
	}
	return (true);
}

bool			ircServer::modeJoinParamsAndFlags(int &modes_to_add, std::vector<std::string> &params,
											users_map::iterator &user, channel_map::iterator it_chan,
											std::string &limit, std::string &user_edit, std::string &pass)
{
	size_t	count = 0;
	bool	L_set = get_bit(modes_to_add, CHAN_MASK_L);
	bool	O_set = get_bit(modes_to_add, CHAN_MASK_O);
	bool	K_set = get_bit(modes_to_add, CHAN_MASK_K);

	if (L_set)
		count++;
	if (O_set)
		count++;
	if (K_set)
		count++;
	if ((params.size() - 2) < count) {
		sendToClient(user->first, ERR_NEEDMOREPARAMS, "MODE");
		return (false);
	}
	if (L_set) {
		limit = params[2];
		if (O_set) {
			user_edit = params[3];
			if (K_set)
				pass = params[4];
		}
		else if (K_set)
			pass = params[3];
	}
	else if (O_set) {
		user_edit = params[2];
		if (K_set)
			pass = params[3];
	}
	else if (K_set)
		pass = params[2];

	if (O_set) {
		if (!it_chan->second.isOperator(user->first)) {
			sendToClient(user->first, ERR_CHANOPRIVSNEEDED, it_chan->first);
			return (false);
		}
		if (!it_chan->second.isOnChannel(this->getUserByNick(user_edit))) {
			sendToClient(user->first, ERR_NOSUCHNICK, user_edit);
			return (false);
		}
		it_chan->second.setUserRole(this->getUserByNick(user_edit), true);
		modes_to_add = clear_bit(modes_to_add, CHAN_MASK_O);
	}
	if (L_set) {
		std::stringstream	ss;
		size_t				num;					
		
		ss << limit;
		ss >> num;
		it_chan->second.setUserLimit(num);
	}
	if (K_set)
		it_chan->second.setPassword(pass);
	return (true);
}
