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

std::string	ircServer::topic(user_id id, std::string current_chan, const char *msg)
{
	/*
		TOPIC REPLIES
			- ERR_NEEDMOREPARAMS
			-x ERR_NOTONCHANNEL (in setDescription())
			-x RPL_NOTOPIC (in setDescription())
			-x RPL_TOPIC
			-x ERR_CHANOPRIVSNEEDED (in setDescription())
	*/
	rplManager				*rpl_manager = rplManager::getInstance();
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
           -x RPL_TOPIC
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
		-x ERR_NEEDMOREPARAMS             -x ERR_NOSUCHCHANNEL
        -x ERR_NOTONCHANNEL
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
           - ERR_NEEDMOREPARAMS              -x ERR_NOSUCHCHANNEL
           - ERR_BADCHANMASK                 -x ERR_CHANOPRIVSNEEDED
           -x ERR_NOTONCHANNEL
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
