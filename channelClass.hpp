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
	std::map<user_id, role>	_users; // nope

	channel() {}

public:
	channel(std::string name, user_id creator) : _name(name), _description("Super channel " + name)
	{
		_users.insert(std::pair<user_id, role>(creator, true));
	}
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