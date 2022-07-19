#include "ircServer.hpp"

void	ircServer::channelNameCorrection(std::string &name)
{
	// RFC said channel name must start with '#' or '&'
	// I think we should remove '#' or '&' in each cmd and add it only for print
	// because with irssi do /join #chan or /join chan are same

	std::string	forbid = CHAN_NAME_FORBID_COMMA;
	forbid += CHAN_NAME_FORBID_CTRLG;
	forbid += CHAN_NAME_FORBID_SPACE;
	size_t		found = name.find_first_of(forbid);

	if (name.size() > CHAN_NAME_MAX_SIZE)
		name = name.substr(0, CHAN_NAME_MAX_SIZE);
	while (found != std::string::npos) {
		name[found] = '_';
		found = name.find_first_of(forbid, found+1);
	}
	std::cout << "CHAN name after correction : " << name << std::endl;
}

ircServer::channel_pair	ircServer::addChannel(std::string name, user_id id)
{
	channel_pair	ret;

	this->channelNameCorrection(name);
	ret = _channel.insert(std::pair<std::string, channel>(name, channel(name, id)));
	if (ret.second == false)
		std::cerr << "ERROR : channel already exist." << std::endl; //! pas de reponse dans la RFC
	return (ret);
}

void	ircServer::removeChannel(channel_map::iterator &it)
{
	it = _channel.erase(it);
}
