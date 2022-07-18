#include "ircServer.hpp"


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

const char	*ircServer::topic(user_id id, std::string current_chan, const char *msg)
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
	std::string				response;
	int						ret;
	channel_map::iterator	it;

	it = _channel.find(current_chan);
	if (it == _channel.end()) {
		std::cerr << "Channel does not exist" << std::endl;
		return (rpl_manager->createResponse(2, current_chan)); //! reponse non indiquée dans RFC
	}
	if (msg == NULL) {
		response = rpl_manager->createResponse(
							RPL_TOPIC,
							current_chan,
							it->second.getDescription().c_str());
		return (response.c_str());
	}
	ret = it->second.setDescription(id, msg);
	if (ret == RPL_TOPIC) {
			response = rpl_manager->createResponse(
							RPL_TOPIC,
							current_chan,
							it->second.getDescription().c_str());
	}
	else {
		response = rpl_manager->createResponse(
							ret,
							current_chan);
	}
	return (response.c_str());
}