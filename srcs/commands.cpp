#include "ircServer.hpp"

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

bool	ircServer::notice(users_map::iterator pair, std::vector<std::string> &argvec) {
	channel_map::iterator	pos;
	int						fd;
	std::string		before = "";
	std::string	dest = argvec[0];

	if (argvec.size() == 0)
		std::cout << "NOTICE doesn't crash\n";
	else if (argvec.size() == 1)
		std::cout << "NOTICE doesn't crash\n";
	else if (argvec.size() > 3)
		std::cout << "NOTICE doesn't crash\n";
	else {
		if (dest[0] == '#') {
			pos = _channel.find(dest);
			if (pos == _channel.end())
				std::cout << "NOTICE doesn't crash\n";
			else if (!pos->second.isOnChannel(pair->first))
				std::cout << "NOTICE doesn't crash\n";
			else {
				dest += " :";
				dest += argvec[1];
				sendToChannel(pair->first, pos->second, RPL_OKNOTICE, before, dest);
				return (true);
			}
		}
		else {
			if ((fd = getUserByNick(dest)) == 0)
				std::cout << "NOTICE doesn't crash\n";
			else {
				dest += " :";
				dest += argvec[1];
				sendToClient(pair->first, fd, RPL_OKNOTICE, std::string(), dest);
				return (true);
			}
		}
	}
	return (false);
}

bool	ircServer::privmsg(users_map::iterator pair, std::vector<std::string> &argvec) {

	channel_map::iterator	pos;
	int						fd;
	std::string		before = "";
	std::string	dest = argvec[0];

	if (argvec.size() == 0)
		sendToClient(pair->first, ERR_NORECIPIENT, "PRIVMSG");
	else if (argvec.size() == 1)
		sendToClient(pair->first, ERR_NOTEXTTOSEND, "PRIVMSG");
	else if (argvec.size() > 3)
		sendToClient(pair->first, ERR_TOOMANYTARGETS, "PRIVMSG");
	else {
		if (dest[0] == '#') {
			pos = _channel.find(dest);
			if (pos == _channel.end())
				sendToClient(pair->first, ERR_NOSUCHCHANNEL, "PRIVMSG");
			else if (!pos->second.isOnChannel(pair->first))
				sendToClient(pair->first, ERR_CANNOTSENDTOCHAN, "PRIVMSG");
			else {
				dest += " :";
				dest += argvec[1];
				sendToChannel(pair->first, pos->second, RPL_OKPRIVMSG, before, dest);
				return (true);
			}
		}
		else {
			if ((fd = getUserByNick(dest)) == 0)
				sendToClient(pair->first, ERR_NOSUCHNICK, "PRIVMSG");
			else {
				dest += " :";
				dest += argvec[1];
				sendToClient(pair->first, fd, RPL_OKPRIVMSG, std::string(), dest);
				return (true);
			}
		}
	}
	return (false);
}

bool    ircServer::nick(users_map::iterator pair, std::vector<std::string> &argvec) {
	std::string			newNick = argvec[0];
    users_map::iterator beg = _users.begin();
    users_map::iterator end = _users.end();
    std::string        	 res;

	if (newNick.length() > 9 || !newNick.length() || !isValid(newNick)) {
        sendToClient(pair->first, ERR_ONEUSNICKNAME, newNick);
		return (false);
    }

    while (beg != end) {
        if (toLower(beg->second.getNick()) == toLower(newNick)) {
            sendToClient(pair->first, ERR_NICKNAMEINUSE, newNick);
			return (false);
        }
        beg++;
    }
	if (pair->second.getStatus() == USER_STATUS_CONNECTED)
     	sendToClient(pair->first, RPL_OKNICK, std::string(), newNick);
    pair->second.setNick(newNick);
	return (true);
}

bool	ircServer::handleUser(users_map::iterator pair, std::vector<std::string> &argvec) {
	std::string			newName = argvec[3];
	std::string			newUserName = argvec[1];
	
	if (pair->second.getStatus() == USER_STATUS_CONNECTED) {
		sendToClient(pair->first, ERR_ALREADYREGISTRED);
		return (false);
	}
	int i = 0;
	while (i <= 3) {
		std::cout << argvec[i] << std::endl;
		i++;
	}
	if (argvec.size() < 4) {	
		sendToClient(pair->first, ERR_NEEDMOREPARAMS);
		return (false);
	}
	pair->second.setName(newName);
	pair->second.setUserName(newUserName);
	return (true);
}

bool		ircServer::pass(users_map::iterator pair, std::vector<std::string> &argvec) {
	std::string	pass = argvec[0];
	if (pass != this->_pass) {
		std::cerr << "Incorrect Password" << std::endl;
		return (false);
	}
	else if (pass.size() < 1) {
		sendToClient(pair->first, ERR_NEEDMOREPARAMS);
		return (false);
	}
	return (true);
}

bool	ircServer::topic(users_map::iterator user, std::vector<std::string> params)
{
	int						ret;
	channel_map::iterator	chan_it;
	std::string				chan;
	bool					read_topic = params.size() < 2 ? true : false;

	if (params.size() == 0) {
		sendToClient(user->first, ERR_NEEDMOREPARAMS, "TOPIC");
		return (false);
	}
	chan = params[0];
	chan_it = _channel.find(chan);
	if (chan_it == _channel.end()) {
		std::cerr << "Channel " << chan << " does not exist" << std::endl;
		return (false);
	}
	if (read_topic) {
		sendToClient(user->first, RPL_TOPIC, chan, chan_it->second.getDescription().c_str());
		return (true);
	}
	ret = chan_it->second.setDescription(user->first, params[1]);
	if (ret == RPL_TOPIC) {
		sendToClient(user->first, RPL_TOPIC, chan, chan_it->second.getDescription().c_str());
		return (true);
	}
	sendToClient(user->first, ret, chan);
	return (false);
}

bool	ircServer::join(users_map::iterator user, std::vector<std::string> params)
{
	std::vector<std::string>	chans;
	std::vector<std::string>	keys;
	channel_map::iterator		chan_exist;
	bool						user_not_in = true;

	if (params.size() == 0) {
		sendToClient(user->first, ERR_NEEDMOREPARAMS, "JOIN");
		return (false);
	}
	chans = split_in_vect(params[0], MSG_MULTI_PARAM_DELIM);
	if (params.size() >= 2) {
		keys = split_in_vect(params[1], MSG_MULTI_PARAM_DELIM);
		if (chans.size() != keys.size()) {
			sendToClient(user->first, ERR_NEEDMOREPARAMS, "JOIN");
			return (false);
		}
	}
	size_t	i = 0;
	while (i < chans.size()) {
		chan_exist = _channel.find(chans[i]);
		if (chan_exist == _channel.end())
			chan_exist = this->addChannel(chans[i], user->first).first;
		else {
			if (chan_exist->second.isFlagSets(CHAN_MASK_I)) {
				if (chan_exist->second.isOnInvitList(user->first)) {
					chan_exist->second.removeFromInvitList(user->first);
				}
				else {
					sendToClient(user->first, ERR_INVITEONLYCHAN, chan_exist->first);
					return (false);
				}
			}
			if (chan_exist->second.isFlagSets(CHAN_MASK_L)) {
				if (chan_exist->second.getUserLimit() <= chan_exist->second.getSize()) {
					sendToClient(user->first, ERR_CHANNELISFULL, chan_exist->first);
					return (false);
				}
			}
			if (chan_exist->second.isFlagSets(CHAN_MASK_K)) {
				if (chans.size() != keys.size()) {
					sendToClient(user->first, ERR_NEEDMOREPARAMS, "JOIN");
					return (false);
				}
				if (chan_exist->second.getPassword() != keys[i]) {
					sendToClient(user->first, ERR_BADCHANNELKEY, chan_exist->first);
					return (false);
				}
			}
			user_not_in = chan_exist->second.addUser(user->first);
		}
		if (user_not_in) {
			sendToClient(user->first, RPL_OKJOIN, std::string(), chan_exist->first);
			topic(user, std::vector<std::string>(1, chan_exist->first));
			names(user, std::vector<std::string>(1, chan_exist->first));
		}
		i++;
	}
	return (true);
}

bool	ircServer::part(users_map::iterator user, std::vector<std::string> params)
{
	std::vector<std::string>					chans;
	channel_map::iterator						chan_exist;
	bool										send_part_msg = true;
	size_t										i = 0;

	if (params.size() == 0) {
		sendToClient(user->first, ERR_NEEDMOREPARAMS, "PART");
		return (false);
	}
	chans = split_in_vect(params[0], MSG_MULTI_PARAM_DELIM);
	if (chans.size() > 1)
		send_part_msg = false;
	while (i < chans.size()) {
		chan_exist = _channel.find(chans[i]);
		if (chan_exist == _channel.end())
			sendToClient(user->first, ERR_NOSUCHCHANNEL, chans[i]);
		else {
			if (chan_exist->second.removeUser(user->first) == 0) {
				sendToClient(user->first, ERR_NOTONCHANNEL, chans[i]);
				return (false);
			}
			if (send_part_msg && params.size() > 1) {
				std::string ret(chan_exist->first + " :" + params[1]);
				sendToClient(user->first, RPL_OKPART, std::string(), ret);
				sendToChannel(user->first, chan_exist->second, RPL_OKPART, std::string(), ret);
			}
			else {
				sendToClient(user->first, RPL_OKPART, std::string(), chan_exist->first);
				sendToChannel(user->first, chan_exist->second, RPL_OKPART, std::string(), chan_exist->first);
			}
			if (chan_exist->second.getSize() == 0)
				_channel.erase(chan_exist);
		}
		i++;
	}
	return (true);
}

bool	ircServer::kick(users_map::iterator user, std::vector<std::string> params)
{
	channel_map::iterator	it_chan;
	user_id					kicker = user->first;
	user_id					victim;
	std::string				victim_nick;
	std::string				cur_chan;
	std::string				comment;
	std::string				res;
	
	if (params.size() < 2) {
		sendToClient(user->first, ERR_NEEDMOREPARAMS, "KICK");
		return (false);
	}
	cur_chan = params[0];
	victim = this->getUserByNick(params[1]);
	comment = params.size() > 2 ? params[2] : "";
	it_chan = _channel.find(cur_chan);
	if (it_chan == _channel.end()) {
		sendToClient(user->first, ERR_NOSUCHCHANNEL, cur_chan);
		return (false);
	}
	if (it_chan->second.isOperator(kicker)) {
		if (!it_chan->second.isOnChannel(victim)) {
			sendToClient(user->first, ERR_NOTONCHANNEL, cur_chan);
			return (false);
		}
		else {
			victim_nick = getUserById(victim)->second.getNick();
			res = cur_chan;
			res += " ";
			res += victim_nick;
			if (comment.size() > 0) {
				res += " :";
				res += comment; 
			}
			sendToClient(kicker, RPL_OKKICK, std::string(), res);
			sendToChannel(kicker, it_chan->second, RPL_OKKICK, std::string(), res);
			it_chan->second.removeUser(victim);
		}
	}
	else {
		sendToClient(user->first, ERR_CHANOPRIVSNEEDED, cur_chan);
		return (false);
	}
	if (it_chan->second.getSize() == 0)
		_channel.erase(it_chan);
	return (true);
}

bool	ircServer::quit(users_map::iterator user, std::vector<std::string> params)
{
	std::string		res(":");

	if (user != _users.end()) {
		if (params.size() > 0)
			res += params[0];
		else
			res += user->second.getNick();
		for (channel_map::iterator chan = _channel.begin(); chan != _channel.end(); chan++) {
			if (chan->second.isOnChannel(user->first))
				sendToChannel(user->second, chan->second, RPL_OKQUIT, std::string(), res);
		}
		user->second.setStatus(USER_STATUS_DEL);
	}
	return (true);
}

bool	ircServer::mode(users_map::iterator user, std::vector<std::string> params)
{
	channel_map::iterator	it_chan;
	int						modes_to_add = 0;
	int						modes_to_remove = 0;
	std::string				limit;
	std::string				user_edit;
	std::string				pass;

	if (params.size() == 0) {
		sendToClient(user->first, ERR_NEEDMOREPARAMS, "MODE");
		return (false);
	}
	if (params.size() > 1) {
		if (params[0][0] != '#' && params[1].find("+i") != std::string::npos) {
			sendToClient(user->first, ERR_UNKNOWNCOMMAND, "MODE +i");
			return (false);
		}
	}
	it_chan = _channel.find(params[0]);
	if (it_chan == _channel.end()) {
		sendToClient(user->first, ERR_NOSUCHCHANNEL, params[0]);
		return (false);
	}
	if (!it_chan->second.isOnChannel(user->first)) {
		sendToClient(user->first, ERR_NOTONCHANNEL, params[0]);
		return (false);
	}
	if (params.size() < 2) {
		sendToClient(user->first, RPL_CHANNELMODEIS, it_chan->first, it_chan->second.convertModeMaskToFlags());
		return (true);
	}
	if (!it_chan->second.isOperator(user->first)) {
		sendToClient(user->first, ERR_CHANOPRIVSNEEDED, it_chan->first);
		return (false);
	}
	modes_to_add = it_chan->second.convertFlagsToMask(params[1], true);
	modes_to_remove = it_chan->second.convertFlagsToMask(params[1], false);
	if (modeRemoveUserOperator(modes_to_remove, params, user, it_chan) == false)
		return (false);
	if (modeJoinParamsAndFlags(modes_to_add, params, user, it_chan, limit, user_edit, pass) == false)
		return (false);
	it_chan->second.removeFlags(modes_to_remove);
	it_chan->second.addFlags(modes_to_add);
	sendToClient(user->first, RPL_CHANNELMODEIS, it_chan->first, it_chan->second.convertModeMaskToFlags());
	return (true);
}

bool	ircServer::names(users_map::iterator user, std::vector<std::string> params)
{
	std::vector<std::string>	chans;
	bool						print_all = !(params.size());
	channel_map::iterator		all_chans_it;

	if (!print_all) {
		chans = split_in_vect(params[0], MSG_MULTI_PARAM_DELIM);
		for (std::vector<std::string>::iterator chan = chans.begin();
			chan != chans.end(); chan++) {
			all_chans_it = _channel.find(*chan);
			if (all_chans_it != _channel.end())
				this->namesRplConditions(user, all_chans_it);
		}
	}
	else {
		all_chans_it = _channel.begin();
		while (all_chans_it != _channel.end()) {
			this->namesRplConditions(user, all_chans_it);
			all_chans_it++;
		}
	}
	return (true);
}

bool	ircServer::list(users_map::iterator user, std::vector<std::string> params)
{
	rplManager					*rpl_manager = rplManager::getInstance();
	std::vector<std::string>	chans;
	bool						print_all = true;
	channel_map::iterator		all_chans_it;

	if (params.size() > 0)
		print_all = !(params[0].size());
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
	sendToClient(user->first, RPL_LISTEND);
	return (true);
}

bool	ircServer::invite(users_map::iterator user, std::vector<std::string> params)
{
	channel_map::iterator		chan_exist;
	user_id						invited_user;
	std::string					rep;

	if (params.size() < 2) {
		sendToClient(user->first, ERR_NEEDMOREPARAMS, "INVITE");
		return (false);
	}
	invited_user = this->getUserByNick(params[0]);
	if (invited_user == 0) {
		sendToClient(user->first, ERR_NOSUCHNICK, params[0]);
		return (false);
	}
	chan_exist = _channel.find(params[1]);
	if (chan_exist != _channel.end()) {
		if (!chan_exist->second.isOnChannel(user->first)) {
			sendToClient(user->first, ERR_NOTONCHANNEL, chan_exist->first);
			return (false);
		}
		if (chan_exist->second.isOnChannel(invited_user)) {
			rep = params[0];
			rep += " ";
			rep += chan_exist->first;
			sendToClient(user->first, ERR_USERONCHANNEL, rep);
			return (false);
		}
		if (chan_exist->second.isFlagSets(CHAN_MASK_I)
			&& !chan_exist->second.isOperator(user->first)) {
			sendToClient(user->first, ERR_CHANOPRIVSNEEDED, chan_exist->first);
			return (false);
		}
		rep = user->second.getNick();
		rep += " ";
		rep += chan_exist->first;
		sendToClient(user->first, RPL_INVITING, chan_exist->first, params[0]);
		sendToClient(user->first, this->getUserByNick(params[0]), RPL_OKNINVITE, std::string(), rep);
		chan_exist->second.addToInvitList(invited_user, params[0]);
		return (true);
	}
	return (false);
}


bool	ircServer::pong(users_map::iterator user, std::vector<std::string> params)
{
	std::string		rep("localhost ");

	for (size_t i = 0; i < params.size(); i++) {
		rep += params[i];
		rep += " ";
	}
	sendToClient(user->first, RPL_OKPONG, std::string(), rep);
	return (true);
}
