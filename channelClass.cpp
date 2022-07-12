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

}

// USAGE
void	channel::sendToAll(ircServer& server, const char *msg) {
	size_t	nb_users = _users.size();
	for (size_t i = 0; i < nb_users; i++)
		server.sendToClient(i, msg);
}