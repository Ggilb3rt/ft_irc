#include "rplManager.hpp"


rplManager	*rplManager::_rplManager = nullptr;

rplManager	*rplManager::getInstance()
{
	if (_rplManager == nullptr)
		_rplManager = new rplManager();
	return _rplManager;
}

rplManager::rplManager()
{
	list.insert(new_pair(1, " :lol internet"));
	list.insert(new_pair(14, " :petit mais puissant"));
	list.insert(new_pair(ERR_NOTONCHANNEL, ERR_NOTONCHANNEL_MSG));
	list.insert(new_pair(ERR_CHANOPRIVSNEEDED, ERR_CHANOPRIVSNEEDED_MSG));
	list.insert(new_pair(ERR_NEEDMOREPARAMS, ERR_NEEDMOREPARAMS_MSG));
	list.insert(new_pair(RPL_NOTOPIC, RPL_NOTOPIC_MSG));
	list.insert(new_pair(RPL_TOPIC, RPL_TOPIC_MSG));
}

rplManager::~rplManager()
{
	std::cout << "Delete replies manager\n";
}

std::string	rplManager::createResponse(int ret, std::string opt_before, std::string opt_after)
{
	std::stringstream	ss;
	std::string				res = "";
	rpl_map::iterator	it = list.find(ret);

	ss << ret;
	for (int i = ret; i < 100; i *= 10) {
		res += '0';
	}
	res += ss.str();
	if (opt_before.size() > 0) {
		res += " ";
		res += opt_before;
	}
	if (it != list.end())
		res += it->second;
	if (opt_after.size() > 0) {
		// res += " ";
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