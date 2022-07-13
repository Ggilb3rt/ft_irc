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
#define user_id int
#define role	bool

private:
	std::string				_name;
	std::string				_description;
	std::vector<user_id>	_users;
	std::map<user_id, role>	_users_map;

	channel() {}

public:
	channel(std::string name) : _name(name), _description("Super channel " + name) {}
	~channel() {}

	// MODIFIERS
	std::string	getName() const {return _name;}
	std::string	getDescription() const {return _description;}
	void		setDescription(std::string description);
	void		addUser(user_id user_fd);
	void		removeUser(user_id user_fd);

	// USAGE
	void		sendToAll(ircServer& server, const char *msg) const;
	//void		sendToOne(const char *msg) const;	


	// DEBUG
	void		printUsers() const;

};


#endif