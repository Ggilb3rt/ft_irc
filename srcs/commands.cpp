#include "ircServer.hpp"

//! Commands should send(), some then must send multiple replies (like join)

// considering the input is an iterator->[client_fd][user], and an already parsed string nick

void	toLower(std::string &nickname) {
	int	i = 0;

	while (nickname.c_str()[i]) {
		if (nickname.c_str()[i] >= 'A' && nickname.c_str()[i] <= 'Z')
			nickname[i] = nickname.c_str()[i] + 32;
		i++;
	}
}

bool	isAlnum(char c) {
	if (c >= '0' && c <= '9')
		return (true);
	if (c >= 'a' && c <= 'z')
		return (true);
	if (c >= 'A' && c <= 'Z')
		return (true);
	return (false);
}

bool	isSpecialOk(char c) {
	if ((c >= '[' && c <= '}') || c == '-')
		return (true);
	return (false);
}

bool	isValid(std::string &nickname) {
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

bool    ircServer::handleNick(users_map::iterator pair, std::vector<std::string> &argvec) {
	std::string			newNick = argvec[0];
    users_map::iterator beg = _users.begin();
    users_map::iterator end = _users.end();
    std::string        	 res;

	// SIZE OF NICK ISNT VALID OR NOT IN SET OF CHARS
	std::cout << "\n---HANDLENICK---\n" << "---> NICKNAME == |" << newNick << "|\n";
	if (newNick.length() > 9 || !newNick.length() || !isValid(newNick)) {
		std::cout << "ERR_ERRONEUSNICKNAME\n";
        sendToClient(pair->first, 432);
		return (false);
    }
	std::cout << "\n---HANDLENICK  2---\n" << "---> NICKNAME == |" << newNick << "|\n";

	// MAKE IT CASE INSENSITIVE
	toLower(newNick);

	std::cout << "\n---HANDLENICK  3---\n" << "---> NICKNAME == |" << newNick << "|\n";

	// CHECK FOR DOUBLON
    while (beg != end) {
        if (beg->second.getNick() == newNick) {
            sendToClient(pair->first, 433);
			std::cout << "ERR_NICKNAMEINUSE\n";
			return (false);
        }
        beg++;
    }
    pair->second.setNick(newNick);
	if (pair->second.getStatus() == USER_STATUS_CONNECTED)
     	sendToClient(pair->first, 001);
	return (true);
}

// bool		ircServer::handleUser(users_map::iterator pair, std::vector<std::string> &argvec) {
	
// }

bool		ircServer::checkPass(std::string pass) {
	std::cout << "In pass cmd, pass == |" << pass << "|\n";
	if (pass != this->_pass) {
		return (false);
	}
	else if (pass.size() < 1) {
		std::cout << "ERR_ERRNUMBARGS\n";
		// sendToClient(pair->first, 461);
		return (false);
	}
	return (true);
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
	/*		
	RFC for modes is fucked, no params needed for +k wtf ?!
	so we will use Parameters: <channel> {[+|-]|o|p|s|i|t|n|m|l|v|k} [<limit>] [<user>] [<password>]

		MODE REPLIES
			-> ERR_NEEDMOREPARAMS
			-> RPL_CHANNELMODEIS
			-> ERR_CHANOPRIVSNEEDED          	-> ERR_NOSUCHNICK
			-> ERR_NOTONCHANNEL               	-x ERR_KEYSET
			-x RPL_BANLIST                   	-x RPL_ENDOFBANLIST
			-x ERR_UNKNOWNMODE               	-> ERR_NOSUCHCHANNEL
			-> ERR_USERSDONTMATCH            	-? RPL_UMODEIS
			-x ERR_UMODEUNKNOWNFLAG
	*/

	rplManager				*rpl_manager = rplManager::getInstance();
	channel_map::iterator	it_chan;
	int						modes_to_add = 0;
	int						modes_to_remove = 0;
	std::string				limit;
	std::string				user_edit;
	std::string				pass;

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
	modes_to_add = it_chan->second.convertPositiveFlagsToMask(params[1]);
	modes_to_remove = it_chan->second.convertNegativeFlagsToMask(params[1]);


	//! CHECK FOR REMOVE user operator
	// check if -o user == user_edit
	//		else return ERR_USERSDONTMATCH
	if (get_bit(modes_to_remove, CHAN_MASK_O)) {
		if (params.size() != 3) {
			std::cout << rpl_manager->createResponse(ERR_NEEDMOREPARAMS, "MODE");
			return (false);
		}
		if (user->first != this->getUserByNick(params[2])) {
			std::cout << rpl_manager->createResponse(ERR_USERSDONTMATCH);
			return (false);
		}
		it_chan->second.setUserRole(this->getUserByNick(user->second.getNick()), false);
		modes_to_remove = clear_bit(modes_to_remove, CHAN_MASK_O);
	}



	//! CHECK FOR ADD
	// check mode add who need params l|o|k
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
		std::cout << rpl_manager->createResponse(ERR_NEEDMOREPARAMS, "MODE");
		return (false);
	}
	/*
	L | O | K | LO | LOK | LK | OK 
	*/
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


	// need to check if user is operator
	if (O_set) {
		if (!it_chan->second.isOperator(user->first)) {
			std::cout << rpl_manager->createResponse(ERR_CHANOPRIVSNEEDED, it_chan->first);
			return (false);
		}
		if (!it_chan->second.isOnChannel(this->getUserByNick(user_edit))) {
			std::cout << rpl_manager->createResponse(ERR_NOSUCHNICK, user_edit);
			return (false);
		}
		it_chan->second.setUserRole(this->getUserByNick(user_edit), true);
		modes_to_add = clear_bit(modes_to_add, CHAN_MASK_O);
	}
	// need to check if limit can atoi()
	if (L_set) {
		std::stringstream	ss;
		size_t				num;					
		
		ss << limit;
		ss >> num;
		it_chan->second.setUserLimit(num);
	}
	// set password
	if (K_set)
		it_chan->second.setPassword(pass);





	//! PRINT DEBUG
	// std::cout << "what is limit " << limit
	//				<< " |user_edit " << user_edit 
	//				<< " |pass " << it_chan->second.getPassword() << std::endl;



	// check if remove flags who need some reset l|k ==> done in removeFlags. Better to do it here ? 

	//! CHANGE MODES
	it_chan->second.removeFlags(modes_to_remove);
	it_chan->second.addFlags(modes_to_add);
	// std::cout << "Channel <" << it_chan->first << "> mode " << it_chan->second.convertModeMaskToFlags() << " | " << it_chan->second._modes << std::endl;
	std::cout << rpl_manager->createResponse(RPL_CHANNELMODEIS, it_chan->first, it_chan->second.convertModeMaskToFlags());
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
