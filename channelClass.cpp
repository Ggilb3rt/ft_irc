#include "ircServer.hpp"
#include "channelClass.hpp"

// MODIFIERS
void		channel::setDescription(std::string description) {
	this->_description = description;
}

void		channel::addUser(user_id user_fd) {
	this->_users.push_back(user_fd);
}

void		channel::removeUser(user_id user_fd) {
	// remove from this->_users
	// user_fd must remove from his _channels, this->getName()
}




// USAGE
void	channel::sendToAll(ircServer& server, const char *msg) const{
	size_t	nb_users = _users.size();
	for (size_t i = 0; i < nb_users; i++)
		server.sendToClient(_users[i], msg);
}




// DEBUG
void	channel::printUsers() const {
	size_t	nb_users = _users.size();
	std::cout << "Users in channel " << _name << " : " << std::endl;
	for (size_t i = 0; i < nb_users; i++)
		std::cout << "\t- " << _users[i] << std::endl;
	std::cout << std::endl;
}