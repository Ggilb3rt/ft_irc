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

friend class ircServer;

typedef		std::map<user_id, role>					users_list;
typedef		std::map<user_id, role>::const_iterator	users_list_const_it;

private:
	const std::string			_name;
	std::string					_description;
	users_list					_users;
	int							_modes;
	size_t						_user_limit;
	std::string					_password;
	std::map<user_id, std::string>	_invit_list;

	channel() {}


public:
	//! should be private (after tests)
	int				convertFlagsToMask(std::string param, bool flag_positive);
	int				convertPositiveFlagsToMask(std::string param);
	int				convertNegativeFlagsToMask(std::string param);

	std::string		convertModeMaskToFlags();
	size_t			getUserLimit() { return _user_limit; }
	void			setUserLimit(size_t new_limit) { _user_limit = new_limit; }
	std::string		getPassword() {return _password; }
	void			setPassword(std::string s) { _password = s; }
	
	//! should be private (after tests)

	channel(std::string name, user_id creator) :
			_name(name), _description("Super channel " + name), _modes(0),
			_user_limit(0), _password(""), _invit_list()
	{
		_users.insert(std::pair<user_id, role>(creator, true));
		(void)_user_limit; (void)_password; (void)_invit_list;
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
	void		addToInvitList(user_id id, std::string nick) { _invit_list.insert(std::pair<user_id, std::string>(id, nick));}
	void		removeFromInvitList(user_id id) { _invit_list.erase(id);}
	bool		isOnInvitList(user_id id) const
	{ 
					if (_invit_list.find(id) != _invit_list.end())
						return true;
					return false;
	}

	users_list::iterator		getUsers() { return _users.begin();}
	users_list::iterator		getEnd() { return _users.end();}
	
	bool		isFlagSets(int flag) const;
	void		addFlags(int flag);
	void		removeFlags(int flag);
	//! not sure if needded
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