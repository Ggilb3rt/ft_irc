#include "ircServer.hpp"
#include "channelClass.hpp"

/*
	Idée : les fonctions returns le message de retour que send() 
	devra envoyer au client

*/


// MODIFIERS
void		channel::setDescription(std::string description) {
	this->_description = description;
}

void		channel::addUser(user_id user_fd) {
	// need to check if already exist
	this->_users.insert(std::pair<user_id, role>(user_fd, false));
}

void		channel::removeUser(user_id user_fd) {
	this->_users.erase(user_fd);
}




// USAGE
void		channel::sendToAll(ircServer& server, const char *msg) const {
	// size_t	nb_users = _users.size();
	// for (size_t i = 0; i < nb_users; i++)
	// 	server.sendToClient(_users[i], msg);
	std::map<user_id, role>::const_iterator	it = _users.begin();

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
		std::cout << "\t- " << it->first << std::endl;
		it++;
	}
	std::cout << std::endl;
}