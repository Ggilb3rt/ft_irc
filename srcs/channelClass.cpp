#include "ircServer.hpp"
#include "channelClass.hpp"

/*
	Idée : les fonctions returns le message de retour que send() 
	devra envoyer au client

*/

int		channel::convertPositiveFlagsToMask(std::string param)
{
	std::string	valid_flags = CHAN_FLAGS_VALID;
	int			mask = 0;
	bool		mask_add = true;

	for (std::string::iterator it = param.begin(); it != param.end(); it++) {
		if (*it == '-')
			mask_add = false;
		else if (*it == '+')
			mask_add = true;
		else if (valid_flags.find(*it) != std::string::npos) {
			if (mask_add)
				mask = set_bit(mask, valid_flags.find(*it));
			else
				mask = clear_bit(mask, valid_flags.find(*it));
		}
	}
	// std::cout << mask << std::endl;
	return (mask);
}

int		channel::convertNegativeFlagsToMask(std::string param)
{

	std::string	valid_flags = CHAN_FLAGS_VALID;
	int			mask = 0;
	bool		mask_add = true;

	for (std::string::iterator it = param.begin(); it != param.end(); it++) {
		if (*it == '-')
			mask_add = false;
		else if (*it == '+')
			mask_add = true;
		else if (valid_flags.find(*it) != std::string::npos) {
			if (mask_add)
				mask = clear_bit(mask, valid_flags.find(*it));
			else
				mask = set_bit(mask, valid_flags.find(*it));
		}
	}
	// std::cout << mask << std::endl;
	return (mask);
}


std::string		channel::convertModeMaskToFlags()
{
	std::string flags;
	std::string	valid_flags = CHAN_FLAGS_VALID;
	int			modes_cpy = this->_modes;
	int			i = 0;

	while (modes_cpy != 0 && i < CHAN_FLAGS_QT) {
		if (isFlagSets(i)) {
			flags += valid_flags[i];
			modes_cpy = clear_bit(modes_cpy, i);
		}
		i++;
	}
	return (flags);
}

bool			channel::isInBanList(std::string nick)
{
	for (std::vector<std::string>::iterator it = this->_banlist.begin() ;
		it != this->_banlist.end() ; it++) {
			if (it->compare(nick) == 0)
				return (true);
				//! send ERR_BANNEDFROMCHAN 467 (maybe not here, in cmd MODE)
	}
	return (false);
}



///////////////
// MODIFIERS //
///////////////
int		channel::setDescription(user_id id, std::string description)
{
	if (!isOnChannel(id))
		return (ERR_NOTONCHANNEL);
	if (isFlagSets(CHAN_MASK_T))
		if (!isOperator(id))
			return (ERR_CHANOPRIVSNEEDED);
	this->_description = description;
	if (this->_description == "")
		return (RPL_NOTOPIC);
	return (RPL_TOPIC);
}

bool	channel::addUser(user_id user_fd)
{
	std::pair<users_list::iterator, bool>	it;

	it = this->_users.insert(std::pair<user_id, role>(user_fd, false));
	if (!it.second) {
		std::cerr << "Warning : User " << user_fd << " already exist in "
		<< this->getName() << std::endl;
		return (false);
	}
	return (true);
}

size_t		channel::removeUser(user_id user_fd) { return (this->_users.erase(user_fd)); }

void		channel::setUserRole(user_id id, role new_role)
{
	// cf OPER command to set correct response (will we do OPER cmd ?)
	users_list::iterator it = _users.find(id);
	if (it != _users.end())
		it->second = new_role;
}

size_t		channel::getSize() const { return (_users.size()); }

// channel::users_list::iterator	channel::getUser(user_id id)
// {
// 	users_list::iterator	it;

// 	it = _users.find(id);
// 	return (it);
// }

void		channel::addFlags(int flag)
{
	int			i = 0;

	while (i < CHAN_FLAGS_QT) {
		if (get_bit(flag, i)) {
			_modes = set_bit(_modes, i);
		}
		i++;
	}
}
void		channel::removeFlags(int flag)
{
	int			i = 0;

	while (i < CHAN_FLAGS_QT) {
		if (get_bit(flag, i)) {
			_modes = clear_bit(_modes, i);
			if (i == CHAN_MASK_L)
				this->setUserLimit(0);
			if (i == CHAN_MASK_K)
				this->setPassword("");
		}
		i++;
	}
	// _modes = clear_bit(_modes, flag);
}
void		channel::toggleFlag(int flag) { _modes = toggle_bit(_modes, flag); }
bool		channel::isFlagSets(int flag) const {return (get_bit(_modes, flag)); }



///////////
// USAGE //
///////////
bool		channel::isOnChannel(user_id id) const
{
	if (_users.find(id) == _users.end())
		return (0);
	return (1);
}

bool		channel::isOperator(user_id id) const
{
	users_list::const_iterator	it = _users.find(id);

	if (it == _users.end())
		return (0);
	if (!it->second)
		return (0);
	return (1);
}


void		channel::replaceLastOperator()
{
	users_list::iterator	it = _users.begin();
	users_list::iterator	end = _users.end();

	while (it != end) {
		if (it->second)
			return ;
		it++;
	}
	this->setUserRole(_users.begin()->first, true);
}

void		channel::sendToAll(ircServer& server, const char *msg) const
{
	std::map<user_id, role>::const_iterator	it = _users.begin();

	// maybe need to add an header to the msg
	while (it != _users.end()) {
		server.sendToClient(it->first, msg);
		it++;
	}
}




// DEBUG
void		channel::printUsers() const
{
	users_list_const_it	it = _users.begin();

	std::cout << "Users in channel " << _name << " : " << std::endl;
	while (it != _users.end()) {
		std::cout << "\t\t- " << it->first << " is " << (it->second ? "\"operator\"" : "\"basic user\"") << std::endl;
		it++;
	}
	std::cout << std::endl;
}