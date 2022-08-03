#include "ircServer.hpp"

//! Commands should send(), some then must send multiple replies (like join)

// considering the input is an iterator->[client_fd][user], and an already parsed string nick

void    ircServer::handleNick(users_map::iterator pair, std::string newNick) {
    users_map::iterator beg = _users.begin();
    users_map::iterator end = _users.end();
    std::string         res;

    while (beg != end) {
        if (beg->second.getNick() == newNick) {
            sendToClient(pair->first, "Nickname already exists !");
            newNick += "_";
            break ;
        }
        beg++;
    }
    // if above 9 ask for another nickname ? or disconnect ?
    if (newNick.length() > 9) {
        sendToClient(pair->first, "Nickname must be less than 10 characters !");
    }
    else {
        pair->second.setNick(newNick);
        sendToClient(pair->first, "Nickname changed :)");
    }
}

bool	ircServer::topic(users_map::iterator user, std::vector<std::string> params)
{
		/*
		TOPIC REPLIES
			-> ERR_NEEDMOREPARAMS
			-> ERR_NOTONCHANNEL (in setDescription())
			-> RPL_NOTOPIC (in setDescription())
			-> RPL_TOPIC
			-> ERR_CHANOPRIVSNEEDED (in setDescription())
	*/
	rplManager				*rpl_manager = rplManager::getInstance();
	int						ret;
	channel_map::iterator	chan_it;
	std::string				chan;
	bool					read_topic = params.size() < 2 ? true : false;

	if (params.size() == 0) {
		std::cout << rpl_manager->createResponse(ERR_NEEDMOREPARAMS, "TOPIC");
		return (false);
	}
	chan = params[0];
	chan_it = _channel.find(chan);
	if (chan_it == _channel.end()) {
		std::cerr << "Channel " << chan << " does not exist" << std::endl;
		//! reponse non indiquée dans RFC, probablement juste ignorer
		//std::cout << (rpl_manager->createResponse(2, chan));
		return (false);
	}
	if (read_topic) {
		std::cout << (rpl_manager->createResponse(
							RPL_TOPIC,
							chan,
							chan_it->second.getDescription().c_str()));
		return (true);
	}
	ret = chan_it->second.setDescription(user->first, params[1]);
	if (ret == RPL_TOPIC) {
			std::cout << (rpl_manager->createResponse(
							RPL_TOPIC,
							chan,
							chan_it->second.getDescription().c_str()));
			return (true);
	}
	std::cout << (rpl_manager->createResponse(ret, chan));
	return (false);
}

//! VOIR AVEC PIERRE SI POSSIBLE D"UTILISER SON SPLIT POUR EVITER LES DOUBLONS
std::vector<std::string>	split_in_vect(std::string base, std::string delimiter)
{
	std::vector<std::string>	ret;
	size_t						found = 0;
	size_t						start = 0;

	while (found != std::string::npos) {
		found = base.find(delimiter, start);
		ret.push_back(base.substr(start, found - start));
		start = found + delimiter.size();
	}
	return (ret);
}

bool	ircServer::join(users_map::iterator user, std::vector<std::string> params)
{
	/*
		JOIN REPLIES
			-> ERR_NEEDMOREPARAMS              -x ERR_BANNEDFROMCHAN
			-> ERR_INVITEONLYCHAN              -> ERR_BADCHANNELKEY
			-> ERR_CHANNELISFULL               -x ERR_BADCHANMASK
			-x ERR_NOSUCHCHANNEL               -x ERR_TOOMANYCHANNELS
			-> RPL_TOPIC
	*/

	rplManager					*rpl_manager = rplManager::getInstance();
	std::vector<std::string>	chans;
	std::vector<std::string>	keys;
	channel_map::iterator		chan_exist;
	bool						user_not_in = true;

	if (params.size() == 0) {
		std::cout << rpl_manager->createResponse(ERR_NEEDMOREPARAMS, "JOIN");
		return (false);
	}
	chans = split_in_vect(params[0], MSG_MULTI_PARAM_DELIM);
	if (params.size() >= 2) {
		keys = split_in_vect(params[1], MSG_MULTI_PARAM_DELIM);
		if (chans.size() != keys.size()) {
			std::cout << rpl_manager->createResponse(ERR_NEEDMOREPARAMS, "JOIN");
			return (false);
		}
	}
	// search if chans chan_exist => create or check if can join
	size_t	i = 0;
	while (i < chans.size()) {
		chan_exist = _channel.find(chans[i]);
		if (chan_exist == _channel.end())
			chan_exist = this->addChannel(chans[i], user->first).first;
		else {
			// check mode invite
			if (chan_exist->second.isFlagSets(CHAN_MASK_I)) {
				std::cout << rpl_manager->createResponse(ERR_INVITEONLYCHAN, chan_exist->first);
				return (false);
			}
			// check mode limit then check limit not reach
			if (chan_exist->second.isFlagSets(CHAN_MASK_L)) {
				if (chan_exist->second.getUserLimit() <= chan_exist->second.getSize()) {
					std::cout << rpl_manager->createResponse(ERR_CHANNELISFULL, chan_exist->first);
					return (false);
				}
			}
			// check user nick not in banlist
				// must use a banMask, *!*@* == ban all users from all serves ==> fuck it
			// check mode pass then check corresponding pass
			if (chan_exist->second.isFlagSets(CHAN_MASK_K)) {
				if (chans.size() != keys.size()) {
					std::cout << rpl_manager->createResponse(ERR_NEEDMOREPARAMS, "JOIN");
					return (false);
				}
				if (chan_exist->second.getPassword() != keys[i]) {
					std::cout << rpl_manager->createResponse(ERR_BADCHANNELKEY, chan_exist->first);
					return (false);
				}
			}
			user_not_in = chan_exist->second.addUser(user->first);
		}
		if (user_not_in)
			std::cout << rpl_manager->createResponse(RPL_TOPIC, chan_exist->first, chan_exist->second.getDescription());
		// send NAMES
		i++;
	}
	return (true);
}

bool	ircServer::part(users_map::iterator user, const std::vector<std::string> params)
{
	/*
		PART REPLIES
		-> ERR_NEEDMOREPARAMS             -> ERR_NOSUCHCHANNEL
        -> ERR_NOTONCHANNEL
	*/
	rplManager									*rpl_manager = rplManager::getInstance();
	std::vector<std::string>::const_iterator	chan = params.begin();
	std::vector<std::string>::const_iterator	end = params.end();
	channel_map::iterator						it_chan;

	if (params.size() == 0) {
		std::cout << rpl_manager->createResponse(ERR_NEEDMOREPARAMS, "PART");
		return (false);
	}

	while (chan != end) {
		it_chan = _channel.find(*chan);
		if (it_chan == _channel.end()) {
			std::cout << rpl_manager->createResponse(ERR_NOSUCHCHANNEL, *chan);
			chan++;
			continue;
		}
		if (it_chan->second.removeUser(user->first) == 0) {
			std::cout << rpl_manager->createResponse(ERR_NOTONCHANNEL, *chan);
		}
		if (it_chan->second.getSize() == 0)
			_channel.erase(it_chan);
		chan++;
	}
	return (true);
}

bool	ircServer::kick(users_map::iterator user, const std::vector<std::string> params)
{
	/*
		KICK REPLIES
           -> ERR_NEEDMOREPARAMS              -> ERR_NOSUCHCHANNEL
           -x ERR_BADCHANMASK (no infos on RFC1459 4.2.8)                 
		   -> ERR_CHANOPRIVSNEEDED
           -> ERR_NOTONCHANNEL
	*/
	rplManager				*rpl_manager = rplManager::getInstance();
	channel_map::iterator	it_chan;
	user_id					kicker = user->first;
	user_id					victim;
	std::string				cur_chan;
	std::string				comment;
	
	if (params.size() < 2) {
		std::cout << rpl_manager->createResponse(ERR_NEEDMOREPARAMS, "KICK");
		return (false);
	}
	cur_chan = params[0];
	victim = this->getUserByNick(params[1]);
	comment = params.size() > 2 ? params[2] : "";
	it_chan = _channel.find(cur_chan);
	if (it_chan == _channel.end()) {
		std::cout << rpl_manager->createResponse(ERR_NOSUCHCHANNEL, cur_chan);
		return (false);
	}
	// check if kicker is operator
	if (it_chan->second.isOperator(kicker)) {
		if (it_chan->second.removeUser(victim) == 0) {
			// check if victim exist
			std::cout << rpl_manager->createResponse(ERR_NOTONCHANNEL, cur_chan);
			return (false);
		}
	}
	else {
		std::cout << rpl_manager->createResponse(ERR_CHANOPRIVSNEEDED, cur_chan);
		return (false);
	}
	if (it_chan->second.getSize() == 0)
		_channel.erase(it_chan);

	// must be send as reponse to the channel ? (ie all other users in the channel)
	std::cout << victim << " kicked by " << kicker;
	if (comment.size() > 0)
		std::cout << " using \"" << comment << "\" has a reason";
	std::cout << std::endl;
	return (true);
}

bool	ircServer::quit(users_map::iterator user, std::vector<std::string> params)
{
	/*
		QUIT REPLIES
			none.
	*/
	if (user != _users.end()) {
		if (params.size() > 0)
			std::cout << "QUIT :" << params[0] << std::endl;
		else
			std::cout << "QUIT :" << user->second.getNick() << std::endl;
		this->removeClient(user);
	}
	return (true);
}

bool	ircServer::mode(users_map::iterator user, std::vector<std::string> params)
{
	//! need to do mode user i because irssi send it at the begining


	//! je dois trouver les flags avec des params avant de set quoi que ce soi :
	//! 	utiliser convertModeFlagsToMask() pour avoir les flags +
	//!		checker si k o l ou (b) est/sont presents
	//!		verifier les parametres en consequences
	//!		return ERR_NEEDMOREPARAMS en cas de problemes

	//!	transformer convertModeFlagsToMask() en deux fonctions : une pour get les + et une pour get les -
	//!	et add ou remove en fonction

	/*

	 Parameters: <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>]
               [<ban mask>]




		MODE REPLIES
			-> ERR_NEEDMOREPARAMS
			- RPL_CHANNELMODEIS
			- ERR_CHANOPRIVSNEEDED          	- ERR_NOSUCHNICK
			-> ERR_NOTONCHANNEL               	-x ERR_KEYSET
			- RPL_BANLIST                   	- RPL_ENDOFBANLIST
			-x ERR_UNKNOWNMODE               	-> ERR_NOSUCHCHANNEL
			- ERR_USERSDONTMATCH            	- RPL_UMODEIS
			-x ERR_UMODEUNKNOWNFLAG
	*/

	/*
		check if params[0] (channel) not exist
			send(ERR_NOSUCHCHANNEL) return
		check if id is not in params[0]
			send(ERR_NOTONCHANNEL) return


		check in params[1] if find char !CHAN_FLAGS_VALID && != '+' && != '-'
			send(ERR_UNKNOWNMODE) return
		
		
		
		
		need to check and convert params[1] :
			+p-si+mz		==> +pm-si		==> +pm-si
			+opsitnmlbvk					==> +psitnm (because o and b limits other modes (don't understand))
			+psitnmlvk						==> +psitnm (because l, v and k needs options)
			+psitnmlvk 5					==> +psitnml (because l == 5)
			+psitnmlvk 5 userInChan			==> +psitnmlv (because v == userInChan)
			+psitnmlvk 5 userNotInChan		==> +psitnml (because userNotInChan)
			+psitnmlvk 5 userInChan pass	==> +psitnmlvk (because k == pass)
			+psitnmlvkb 5 userInChan pass	==> +psitnmlvk (send error ban)


		then this->convertModeFlagsToMask(params);
	*/
	rplManager				*rpl_manager = rplManager::getInstance();
	channel_map::iterator	it_chan;
	int						mode;

	if (params.size() < 2) {
		std::cout << rpl_manager->createResponse(ERR_NEEDMOREPARAMS, "MODE");
		return (false);
	}
	it_chan = _channel.find(params[0]);
	if (it_chan == _channel.end()) {
		std::cout << rpl_manager->createResponse(ERR_NOSUCHCHANNEL, params[0]);
		return (false);
	}
	if (!it_chan->second.isOnChannel(user->first)) {
		std::cout << rpl_manager->createResponse(ERR_NOTONCHANNEL, params[0]);
		return (false);
	}
	mode = it_chan->second.convertModeFlagsToMask(params[1]);


	it_chan->second._modes = mode;
	// it_chan->second.addFlag(mode);
	std::cout << "Channel <" << it_chan->first << "> mode " << it_chan->second.convertModeMaskToFlags() << " | " << it_chan->second._modes << std::endl;

	(void)user;
	return (true);
}



bool	ircServer::names(users_map::iterator user, std::vector<std::string> params)
{
	/*
		NAMES REPLIES
        	-> RPL_NAMREPLY
			-> RPL_ENDOFNAMES
	*/

	rplManager					*rpl_manager = rplManager::getInstance();
	std::vector<std::string>	chans;
	bool						print_all = !(params.size());
	channel_map::iterator		all_chans_it;

	if (!print_all) {
		chans = split_in_vect(params[0], MSG_MULTI_PARAM_DELIM);
		for (std::vector<std::string>::iterator chan = chans.begin();
			chan != chans.end(); chan++) {
			all_chans_it = _channel.find(*chan);
			if (all_chans_it != _channel.end())
				this->namesRplConditions(user, all_chans_it, rpl_manager);
		}
	}
	else {
		all_chans_it = _channel.begin();
		while (all_chans_it != _channel.end()) {
			this->namesRplConditions(user, all_chans_it, rpl_manager);
			all_chans_it++;
		}
	}
	return (true);
}

bool	ircServer::list(users_map::iterator user, std::vector<std::string> params)
{
	/*
		LIST REPLIES
			-x ERR_NOSUCHSERVER               -x RPL_LISTSTART (obsolete. Not used.)
           	-> RPL_LIST                       -> RPL_LISTEND
	*/
	
	rplManager					*rpl_manager = rplManager::getInstance();
	std::vector<std::string>	chans;
	bool						print_all = !(params.size());
	channel_map::iterator		all_chans_it;

	if (!print_all) {
		chans = split_in_vect(params[0], MSG_MULTI_PARAM_DELIM);
		for (std::vector<std::string>::iterator chan = chans.begin();
			chan != chans.end(); chan++) {
			all_chans_it = _channel.find(*chan);
			if (all_chans_it != _channel.end()) {
				this->listRplConditions(user, all_chans_it, rpl_manager);
			}
		}
	}
	else {
		all_chans_it = _channel.begin();
		while (all_chans_it != _channel.end()) {
			this->listRplConditions(user, all_chans_it, rpl_manager);
			all_chans_it++;
		}
	}
	std::cout << rpl_manager->createResponse(RPL_LISTEND);
	return (true);
}

bool	ircServer::invite(users_map::iterator user, std::vector<std::string> params)
{
	/*
		LIST REPLIES
          -> ERR_NEEDMOREPARAMS             -> ERR_NOSUCHNICK
          -> ERR_NOTONCHANNEL               -> ERR_USERONCHANNEL
          -> ERR_CHANOPRIVSNEEDED
          -> RPL_INVITING                   -x RPL_AWAY (works with )
	*/

	rplManager					*rpl_manager = rplManager::getInstance();
	channel_map::iterator		chan_exist;
	user_id						invited_user;
	std::string					rep;

	if (params.size() < 2) {
		std::cout << rpl_manager->createResponse(ERR_NEEDMOREPARAMS, "INVITE");
		return (false);
	}
	invited_user = this->getUserByNick(params[0]);
	if (invited_user == 0) {
		std::cout << rpl_manager->createResponse(ERR_NOSUCHNICK, params[0]);
		return (false);
	}
	chan_exist = _channel.find(params[1]);
	if (chan_exist != _channel.end()) {
		if (!chan_exist->second.isOnChannel(user->first)) {
			std::cout << rpl_manager->createResponse(ERR_NOTONCHANNEL, chan_exist->first);
			return (false);
		}
		if (chan_exist->second.isOnChannel(invited_user)) {
			rep = params[0];
			rep += " ";
			rep += chan_exist->first;
			std::cout << rpl_manager->createResponse(ERR_USERONCHANNEL, rep);
			return (false);
		}
		if (chan_exist->second.isFlagSets(CHAN_MASK_I)
			&& !chan_exist->second.isOperator(user->first)) {
			std::cout << rpl_manager->createResponse(ERR_CHANOPRIVSNEEDED, chan_exist->first);
			return (false);
		}
		rep = chan_exist->first;
		rep += " ";
		rep += params[0];
		std::cout << rpl_manager->createResponse(RPL_INVITING, rep);
		return (true);
	}
	return (false);
}
