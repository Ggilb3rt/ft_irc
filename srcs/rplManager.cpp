#include "rplManager.hpp"


rplManager	*rplManager::_rplManager = NULL;

rplManager	*rplManager::getInstance()
{
	if (_rplManager == NULL)
		_rplManager = new rplManager();
	return _rplManager;
}

rplManager::rplManager()
{
	list.insert(new_pair(RPL_OKCONN, RPL_OKCONN_MSG));
	list.insert(new_pair(RPL_OKNICK, RPL_OKNICK_MSG));
	list.insert(new_pair(RPL_OKJOIN, RPL_OKJOIN_MSG));
	list.insert(new_pair(RPL_OKTOPIC, RPL_OKTOPIC_MSG));
	list.insert(new_pair(RPL_OKPONG, RPL_OKPONG_MSG));
	list.insert(new_pair(RPL_OKPART, RPL_OKPART_MSG));
	list.insert(new_pair(RPL_OKNAMES, RPL_OKNAMES_MSG));
	list.insert(new_pair(RPL_OKNLIST, RPL_OKNLIST_MSG));
	list.insert(new_pair(RPL_OKNINVITE, RPL_OKNINVITE_MSG));
	list.insert(new_pair(RPL_OKQUIT, RPL_OKQUIT_MSG));
	list.insert(new_pair(RPL_OKKICK, RPL_OKKICK_MSG));
	list.insert(new_pair(RPL_OKMODE, RPL_OKMODE_MSG));
	list.insert(new_pair(ERR_CANNOTSENDTOCHAN, ERR_CANNOTSENDTOCHAN_MSG));
	list.insert(new_pair(ERR_NORECIPIENT, ERR_NORECIPIENT_MSG));
	list.insert(new_pair(ERR_TOOMANYTARGETS, ERR_TOOMANYTARGETS_MSG));
	list.insert(new_pair(ERR_NOTEXTTOSEND, ERR_NOTEXTTOSEND_MSG));
	list.insert(new_pair(ERR_NOSUCHCHANNEL, ERR_NOSUCHCHANNEL_MSG));
	list.insert(new_pair(ERR_ALREADYREGISTRED, ERR_ALREADYREGISTRED_MSG));
	list.insert(new_pair(ERR_NOSUCHNICK, ERR_NOSUCHNICK_MSG));
	list.insert(new_pair(ERR_ONEUSNICKNAME, ERR_ONEUSNICKNAME_MSG));
	list.insert(new_pair(ERR_NICKNAMEINUSE, ERR_NICKNAMEINUSE_MSG));
	list.insert(new_pair(ERR_USERONCHANNEL, ERR_USERONCHANNEL_MSG));
	list.insert(new_pair(ERR_NOTONCHANNEL, ERR_NOTONCHANNEL_MSG));
	list.insert(new_pair(ERR_NOSUCHCHANNEL, ERR_NOSUCHCHANNEL_MSG));
	list.insert(new_pair(ERR_CHANOPRIVSNEEDED, ERR_CHANOPRIVSNEEDED_MSG));
	list.insert(new_pair(ERR_NEEDMOREPARAMS, ERR_NEEDMOREPARAMS_MSG));
	list.insert(new_pair(ERR_CHANNELISFULL, ERR_CHANNELISFULL_MSG));
	list.insert(new_pair(14, " :petit mais puissant"));
	list.insert(new_pair(ERR_INVITEONLYCHAN, ERR_INVITEONLYCHAN_MSG));
	list.insert(new_pair(ERR_BADCHANNELKEY, ERR_BADCHANNELKEY_MSG));
	list.insert(new_pair(RPL_UMODEIS, RPL_UMODEIS_MSG));
	list.insert(new_pair(RPL_CHANNELMODEIS, RPL_CHANNELMODEIS_MSG));
	list.insert(new_pair(RPL_NOTOPIC, RPL_NOTOPIC_MSG));
	list.insert(new_pair(RPL_TOPIC, RPL_TOPIC_MSG));
	list.insert(new_pair(RPL_LIST, RPL_LIST_MSG));
	list.insert(new_pair(RPL_LISTEND, RPL_LISTEND_MSG));
	list.insert(new_pair(RPL_NAMREPLY, RPL_NAMREPLY_MSG));
	list.insert(new_pair(RPL_ENDOFNAMES, RPL_ENDOFNAMES_MSG));
	list.insert(new_pair(RPL_OKPRIVMSG, RPL_OKPRIVMSG_MSG));
	list.insert(new_pair(RPL_OKNOTICE, RPL_OKNOTICE_MSG));
	list.insert(new_pair(RPL_INVITING, RPL_INVITING_MSG));
	list.insert(new_pair(ERR_USERSDONTMATCH, ERR_USERSDONTMATCH_MSG));
	list.insert(new_pair(ERR_UNKNOWNCOMMAND, ERR_UNKNOWNCOMMAND_MSG));

}

rplManager::~rplManager()
{
	std::cout << "Delete replies manager\n";
}

std::string	rplManager::createResponse(user &usr, int ret, std::string opt_before, std::string opt_after)
{
	std::stringstream	ss;
	std::string res(":" + usr.getNick() + "!" + usr.getUserName() + "@" + "localhost ");
	rpl_map::iterator	it = list.find(ret);

	if (ret > -1) {
		ss << ret;
		for (int i = ret; i < 100; i *= 10) {
			res += '0';
		}
		res += ss.str();
		res += " " + usr.getNick() + " ";
	}
	if (opt_before.size() > 0) {
		res += opt_before;
	}
	if (it != list.end())
		res += it->second;
	if (opt_after.size() > 0) {
		res += opt_after;
	}
	res += END_MSG;
	return (res);
}

void		rplManager::printAll()
{
	rpl_map::iterator start = list.begin();
	rpl_map::iterator end = list.end();

	while (start != end) {
		std::cout << start->first << " => " << start->second << std::endl;
		start++;
	}
}
