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
	const std::string			_name;
	std::string					_description;
	users_list					_users;
	int							_modes;
	int							_user_limit;
	std::string					_password;
	std::vector<std::string>	_banlist;

	channel() {}


public:
	int				convertModeFlagsToMask(std::string param); //! should be private (after tests)
	std::string		convertModeMaskToFlags();	//! should be private (after tests)
	int				getUserLimit() { return _user_limit; }
	std::string		getPassword() {return _password; }
	bool			isInBanList(std::string nick);


	channel(std::string name, user_id creator) :
			_name(name), _description("Super channel " + name), _modes(0),
			_user_limit(0), _password(""), _banlist()
	{
		_users.insert(std::pair<user_id, role>(creator, true));
		(void)_user_limit; (void)_password; (void)_banlist;
	}
	~channel() {}

	// MODIFIERS
	std::string	getName() const {return _name;}
	std::string	getDescription() const {return _description;}
	int			setDescription(user_id id, std::string description);
	bool		addUser(user_id user_fd);
	size_t		removeUser(user_id user_fd);
	void		setUserRole(user_id id, role new_role);
	size_t		getSize() const;
	// users_list::iterator		getUser(user_id id);
	
	bool		isFlagSets(int flag) const;
	//! not sure if needded
	void		addFlag(int flag);
	void		removeFlag(int flag);
	void		toggleFlag(int flag);


	// USAGE
	bool		isOnChannel(user_id id) const;
	bool		isOperator(user_id id) const;
	void		replaceLastOperator();
	void		sendToAll(ircServer& server, const char *msg) const;
	//void		sendToOne(const char *msg) const;	

	// DEBUG
	void		printUsers() const;

};


#endif