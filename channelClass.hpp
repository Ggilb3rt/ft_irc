#ifndef CHANNEL_CLASS_HPP
	#define CHANNEL_CLASS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "ircServer.hpp"
// #include "user_class.hpp"

class ircServer;

class channel
{
#define user int

private:
	std::string			_name;
	std::string			_description;
	std::vector<user>	_users;
	// friend				ircServer;

	channel() {}

public:
	channel(std::string name) : _name(name), _description("Super channel " + name) {}
	~channel() {}

	// MODIFIERS
	std::string	getName() const {return _name;}
	std::string	getDescription() const {return _description;}
	void		setDescription(std::string description);
	void		addUser(user user_fd);
	void		removeUser(user user_fd);

	// USAGE
	void		sendToAll(const char *msg) {
		size_t	nb_users = _users.size();
		for (size_t i = 0; i < nb_users; i++)
			ircServer::sendToClient(i, msg);
	}

};


#endif