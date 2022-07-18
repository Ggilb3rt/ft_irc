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

typedef		std::map<user_id, role>	users_list;

private:
	std::string				_name;
	std::string				_description;
	users_list				_users;

	channel() {}

	bool		isOnChannel(user_id id);
	bool		isOperator(user_id id);

public:
	channel(std::string name, user_id creator) : _name(name), _description("Super channel " + name)
	{
		_users.insert(std::pair<user_id, role>(creator, true));
	}
	~channel() {}

	// MODIFIERS
	std::string	getName() const {return _name;}
	std::string	getDescription() const {return _description;}
	int			setDescription(user_id id, std::string description);
	bool		addUser(user_id user_fd);
	void		removeUser(user_id user_fd);
	void		setUserRole(user_id id, role new_role);
	size_t		getSize();
	// users_list::iterator		getUser(user_id id);


	// USAGE
	void		replaceLastOperator();
	void		sendToAll(ircServer& server, const char *msg) const;
	//void		sendToOne(const char *msg) const;	

	// DEBUG
	void		printUsers() const;

};


#endif