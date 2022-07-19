#ifndef USER_CLASS_HPP
# define USER_CLASS_HPP

#include <string>
#include <map>

class user
{
	private:
		int						_id;
		std::string				_nick;
		std::string				_name;


	public:
		std::string	_msg;

		user(int fd) :
			_id(fd)
		{}
		user(int fd, std::string nick, std::string name) :
			_id(fd),
			_nick(nick),
			_name(name),
			_msg()
		{}
		~user() {}

		int			getId() const {return _id;}
		std::string	getNick() const { return _nick; }
		void		setNick(std::string nick) { this->_nick = nick; }
		std::string	getName() const { return _name; }
		void		setName(std::string name) { this->_name = name; }

};

#endif
