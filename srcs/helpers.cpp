#include "ircServer.hpp"

void	ircServer::sendToClient(int fd, int code, std::string param_1, std::string param_2)
{
	rplManager	*instance = rplManager::getInstance();
	std::string	res	= instance->createResponse(code, param_1, param_2);
    if (send(fd, res.c_str(), res.length(), 0) == -1) {
        std::cerr << strerror(errno) << std::endl;
    }
}

user_id	ircServer::getUserByNick(const std::string nick) const
{
	// users_map::iterator		it = _users.begin();
	// users_map::iterator		end = _users.end();

	// while (it != end) {
	// 	if (it->second.getNick() == nick)
	// 		return (it->first);
	// 	it++;
	// }
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

void	ircServer::namesRplConditions(users_map::iterator &user, channel_map::iterator &all_chans_it, rplManager *rpl_manager)
{
	users_map::iterator			current_user;

	if (all_chans_it->second.isFlagSets(CHAN_MASK_P)
		|| all_chans_it->second.isFlagSets(CHAN_MASK_S)) {
		if (!all_chans_it->second.isOnChannel(user->first))
			return ;
	}
	for (channel::users_list_const_it user_in_chan = all_chans_it->second._users.begin();
		user_in_chan != all_chans_it->second._users.end(); user_in_chan++) {
		current_user = _users.find(user_in_chan->first);
		if (current_user != _users.end())
			std::cout << rpl_manager->createResponse(RPL_NAMREPLY,
													all_chans_it->first,
													current_user->second.getNick());
	}
	std::cout << rpl_manager->createResponse(RPL_ENDOFNAMES, all_chans_it->first);
}

void	ircServer::listRplConditions(users_map::iterator &user, channel_map::iterator &all_chans_it, rplManager *rpl_manager)
{
	if (all_chans_it->second.isFlagSets(CHAN_MASK_P)) {
		if (all_chans_it->second.isOnChannel(user->first))
			std::cout << rpl_manager->createResponse(RPL_LIST, all_chans_it->first, all_chans_it->second.getDescription());
		else
			std::cout << rpl_manager->createResponse(RPL_LIST, all_chans_it->first, "Prv");
	}
	else if (all_chans_it->second.isFlagSets(CHAN_MASK_S)) {
		if (all_chans_it->second.isOnChannel(user->first))
			std::cout << rpl_manager->createResponse(RPL_LIST, all_chans_it->first, all_chans_it->second.getDescription());
	}
	else
		std::cout << rpl_manager->createResponse(RPL_LIST, all_chans_it->first, all_chans_it->second.getDescription());
}


// just for debug
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
	char			_ipstr[INET6_ADDRSTRLEN];
	struct addrinfo	*_p;

	std::cout << std::endl << "IP addresses for " << ADDRESS_NAME << " : " << std::endl << std::endl;
	for(_p = _servinfo;_p != NULL; _p = _p->ai_next) {
		void		*addr;
		std::string	ipver;

		// get the pointer to the address itself,
		// different fields in IPv4 and IPv6:
		if (_p->ai_family == AF_INET) { // IPv4
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)_p->ai_addr;
			std::cout << "Port is " << ntohs(ipv4->sin_port) << std::endl;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		} else { // IPv6
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)_p->ai_addr;
			std::cout << "Port is " << ntohs(ipv6->sin6_port) << std::endl;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		// convert the IP to a string and print it:
		inet_ntop(_p->ai_family, addr, _ipstr, sizeof _ipstr);
		std::cout << "\t" << ipver << " : " << _ipstr << std::endl;
	}
}
