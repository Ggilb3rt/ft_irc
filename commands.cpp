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

std::string	ircServer::topic(user_id id, std::string current_chan, const char *msg)
{
	/*
		TOPIC REPLIES
			- ERR_NEEDMOREPARAMS
			- ERR_NOTONCHANNEL
			- RPL_NOTOPIC
			- RPL_TOPIC
			- ERR_CHANOPRIVSNEEDED
	*/
	rplManager				*rpl_manager = rplManager::getInstance();
	// std::string				response;
	int						ret;
	channel_map::iterator	it;

	it = _channel.find(current_chan);
	if (it == _channel.end()) {
		std::cerr << "Channel does not exist" << std::endl;
		return (rpl_manager->createResponse(2, current_chan)); //! reponse non indiquée dans RFC
	}
	if (msg == NULL) {
		return (rpl_manager->createResponse(
							RPL_TOPIC,
							current_chan,
							it->second.getDescription().c_str()));
	}
	ret = it->second.setDescription(id, msg);
	if (ret == RPL_TOPIC) {
			return (rpl_manager->createResponse(
							RPL_TOPIC,
							current_chan,
							it->second.getDescription().c_str()));
	}
	return (rpl_manager->createResponse(ret, current_chan));
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
           - RPL_TOPIC
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
	// must send RPL_NAMREPLY too (send list of users in channel exist->second.getUsersNick())
}

std::string ircServer::part(user_id id, const std::vector<std::string> chans)
{
	/*
		PART REPLIES
		- ERR_NEEDMOREPARAMS             - ERR_NOSUCHCHANNEL
        - ERR_NOTONCHANNEL
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
		chan++;
	}
	return ("part");
}

std::string ircServer::kick(std::string chan, user_id id, std::string comment)
{
	return ("kick");
}