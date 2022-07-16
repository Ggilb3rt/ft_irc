#include "ircServer.hpp"
#include "channelClass.hpp"

/*
	Idée : les fonctions returns le message de retour que send() 
	devra envoyer au client

*/


bool		channel::isOnChannel(user_id id)
{
	if (_users.find(id) == _users.end())
		return (0);
	return (1);
}

bool		channel::isOperator(user_id id)
{
	users_list::iterator	it = _users.find(id);

	if (it == _users.end())
		return (0);
	if (!it->second)
		return (0);
	return (1);
}

// MODIFIERS
int		channel::setDescription(user_id id, std::string description) {
	if (!isOnChannel(id))
		return (ERR_NOTONCHANNEL);
	if (!isOperator(id))
		return (ERR_CHANOPRIVSNEEDED);
	this->_description = description;
	if (this->_description == "")
		return (RPL_NOTOPIC);
	return (RPL_TOPIC);
}

void		channel::addUser(user_id user_fd) {
	std::pair<users_list::iterator, bool>	it;

	it = this->_users.insert(std::pair<user_id, role>(user_fd, false));
	if (!it.second)
		std::cout << "User " << user_fd << " already exist" << std::endl;
}

void		channel::removeUser(user_id user_fd) {
	this->_users.erase(user_fd);
}

void		channel::setUserRole(user_id id, role new_role) {
	// cf OPER command to set correct response (will we do OPER cmd ?)
	users_list::iterator it = _users.find(id);
	if (it != _users.end())
		it->second = new_role;
}




// USAGE
void		channel::sendToAll(ircServer& server, const char *msg) const {
	std::map<user_id, role>::const_iterator	it = _users.begin();

	// maybe need to add an header to the msg
	while (it != _users.end()) {
		server.sendToClient(it->first, msg);
		it++;
	}
}





// DEBUG
void		channel::printUsers() const {
	std::map<user_id, role>::const_iterator	it = _users.begin();

	std::cout << "Users in channel " << _name << " : " << std::endl;
	while (it != _users.end()) {
		std::cout << "\t- " << it->first << " is " << it->second << std::endl;
		it++;
	}
	std::cout << std::endl;
}