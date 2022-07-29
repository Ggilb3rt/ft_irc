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


bool	ircServer::join(users_map::iterator user, std::vector<std::string> params)
{
	// 1.  the user must be invited if the channel is invite-only;
	// 2.  the user's nick/username/hostname must not match any
	//     active bans;
	// 3.  the correct key (password) must be given if it is set.


	/*
		JOIN REPLIES
			- ERR_NEEDMOREPARAMS              - ERR_BANNEDFROMCHAN
			- ERR_INVITEONLYCHAN              - ERR_BADCHANNELKEY
			- ERR_CHANNELISFULL               - ERR_BADCHANMASK
			- ERR_NOSUCHCHANNEL               - ERR_TOOMANYCHANNELS
			-> RPL_TOPIC
	*/

	(void)user; (void)params;

	// rplManager				*rpl_manager = rplManager::getInstance();
	// channel_map::iterator	exist;
	

	// exist = _channel.find(chan);
	// if (exist == _channel.end())
	// 	exist = this->addChannel(chan, id).first;
	// else {
	// 	if (!(exist->second.addUser(id)))
	// 		return (true); // user already in, just ignore
	// }
	// std::cout << rpl_manager->createResponse(RPL_TOPIC, chan, exist->second.getDescription());
	
	// (void)key;
	// must send RPL_NAMREPLY too (send list of users in channel exist->second.getUsersNick())
	return (true);
}

std::string	ircServer::join(user_id id, std::string chan, std::string key)
{
	// 1.  the user must be invited if the channel is invite-only;
    // 2.  the user's nick/username/hostname must not match any
    //     active bans;
    // 3.  the correct key (password) must be given if it is set.


	/*
		JOIN REPLIES
           - ERR_NEEDMOREPARAMS              - ERR_BANNEDFROMCHAN
           - ERR_INVITEONLYCHAN              - ERR_BADCHANNELKEY
           - ERR_CHANNELISFULL               - ERR_BADCHANMASK
           - ERR_NOSUCHCHANNEL               - ERR_TOOMANYCHANNELS
           -> RPL_TOPIC
	*/


	rplManager				*rpl_manager = rplManager::getInstance();
	channel_map::iterator	exist;
	

	exist = _channel.find(chan);
	if (exist == _channel.end())
		exist = this->addChannel(chan, id).first;
	else {
		if (!(exist->second.addUser(id)))
			return ("..."); // user already in
	}
	return (rpl_manager->createResponse(RPL_TOPIC, chan, exist->second.getDescription()));
	(void)key;
	// must send RPL_NAMREPLY too (send list of users in channel exist->second.getUsersNick())
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
		//must send ERR_NEEDMOREPARAMS
		std::cout << rpl_manager->createResponse(ERR_NEEDMOREPARAMS, "PART");
		return (false);
	}

	while (chan != end) {
		it_chan = _channel.find(*chan);
		if (it_chan == _channel.end()) {
			// must send ERR_NOSUSHCHANNEL not print
			std::cout << rpl_manager->createResponse(ERR_NOSUCHCHANNEL, *chan);
			chan++;
			continue;
		}
		if (it_chan->second.removeUser(user->first) == 0) {
			// must send ERR_NOTONCHANNEL not print
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
	(void)user; (void)params;
	/*
		MODE REPLIES
			ERR_NEEDMOREPARAMS
			RPL_CHANNELMODEIS
			ERR_CHANOPRIVSNEEDED          	ERR_NOSUCHNICK
		-> ERR_NOTONCHANNEL               	ERR_KEYSET
			RPL_BANLIST                   	RPL_ENDOFBANLIST
		-> ERR_UNKNOWNMODE               	-> ERR_NOSUCHCHANNEL
			ERR_USERSDONTMATCH            	RPL_UMODEIS
			ERR_UMODEUNKNOWNFLAG
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
	return (true);
}




std::string	ircServer::names(std::vector<std::string> chans())
{
	/*
		NAMES REPLIES
        	- RPL_NAMREPLY
			- RPL_ENDOFNAMES
	*/

	// if chans.size() == 0 ==> print all
	(void)chans;
	return ("names");
}

std::string	ircServer::list(std::vector<std::string> chans())
{
	/*
		LIST REPLIES
			- ERR_NOSUCHSERVER               - RPL_LISTSTART (obsolete. Not used.)
           	- RPL_LIST                       - RPL_LISTEND
	*/

	// if chans.size() == 0 ==> print all

	// need RPL_LIST
	(void)chans;
	return ("list");
}
