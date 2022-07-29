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
		//std::cout << (rpl_manager->createResponse(2, chan)); //! reponse non indiquée dans RFC, probablement juste ignorer
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

std::string ircServer::part(user_id id, const std::vector<std::string> chans)
{
	/*
		PART REPLIES
		-> ERR_NEEDMOREPARAMS             -> ERR_NOSUCHCHANNEL
        -> ERR_NOTONCHANNEL
	*/
	rplManager									*rpl_manager = rplManager::getInstance();
	std::vector<std::string>::const_iterator	chan = chans.begin();
	std::vector<std::string>::const_iterator	end = chans.end();
	channel_map::iterator						it_chan;

	if (chans.size() == 0) {
		//must send ERR_NEEDMOREPARAMS
		std::cout << rpl_manager->createResponse(ERR_NEEDMOREPARAMS, "PART");
		return ("Part nope");
	}

	while (chan != end) {
		it_chan = _channel.find(*chan);
		if (it_chan == _channel.end()) {
			// must send ERR_NOSUSHCHANNEL not print
			std::cout << rpl_manager->createResponse(ERR_NOSUCHCHANNEL, *chan);
			chan++;
			continue;
		}
		if (it_chan->second.removeUser(id) == 0) {
			// must send ERR_NOTONCHANNEL not print
			std::cout << rpl_manager->createResponse(ERR_NOTONCHANNEL, *chan);
		}
		if (it_chan->second.getSize() == 0)
			_channel.erase(it_chan);
		chan++;
	}
	return ("part");
}

std::string ircServer::kick(std::string chan, user_id victim, user_id kicker, std::string comment)
{
	/*
		KICK REPLIES
           - ERR_NEEDMOREPARAMS              -> ERR_NOSUCHCHANNEL
           - ERR_BADCHANMASK                 -> ERR_CHANOPRIVSNEEDED
           -> ERR_NOTONCHANNEL
	*/
	rplManager									*rpl_manager = rplManager::getInstance();
	channel_map::iterator						it_chan;
	
	it_chan = _channel.find(chan);
	if (it_chan == _channel.end()) {
		std::cout << rpl_manager->createResponse(ERR_NOSUCHCHANNEL, chan);
		return ("ERR_NOSUCHCHANNEL");
	}
	// check if kicker is operator
	if (it_chan->second.isOperator(kicker)) {
		if (it_chan->second.removeUser(victim) == 0) {
			// check if victim exist
			// must send ERR_NOTONCHANNEL not print
			std::cout << rpl_manager->createResponse(ERR_NOTONCHANNEL, chan);
			return ("ERR_NOTONCHANNEL");
		}
	}
	else {
		std::cout << rpl_manager->createResponse(ERR_CHANOPRIVSNEEDED, chan);
		return ("ERR_CHANOPRIVSNEEDED");
	}
	if (it_chan->second.getSize() == 0)
		_channel.erase(it_chan);

	// must be send as reponse ?
	std::cout << victim << " kicked by " << kicker;
	if (comment.size() > 0)
		std::cout << " using " << comment << " has a reason";
	std::cout << std::endl; 
	return ("kick");
}

std::string	ircServer::quit(user_id client, std::string message)
{
	users_map::iterator	user_it = this->getUserById(client);

	if (user_it != _users.end()) {
		this->removeClient(user_it);
		if (message.size() > 0)
			std::cout << "QUIT :" << message << std::endl;
		else
			std::cout << "QUIT :" << user_it->second.getName() << std::endl;
	}
	return ("quit");
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
