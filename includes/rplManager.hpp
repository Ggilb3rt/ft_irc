#ifndef REPLIES_HPP
	#define REPLIES_HPP

#include "irc_protocole_rules.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <map>

// ERRORS
#define ERR_NOSUCHCHANNEL			403
#define ERR_NOSUCHCHANNEL_MSG		" :No such channel" // 403 <channel_name>
#define ERR_NOTONCHANNEL			442
#define ERR_NOTONCHANNEL_MSG		" :You're not on that channel" // 442 <channel>
#define ERR_CHANOPRIVSNEEDED		482
#define ERR_CHANOPRIVSNEEDED_MSG	" :You are not channel operator" // 482 <channel>
#define ERR_NEEDMOREPARAMS			461
#define ERR_NEEDMOREPARAMS_MSG		" :Not enough parameters" // 461 <command>

// REPLIES
#define RPL_OKCONNECT		001
#define RPL_OKCONNECT_MSG	" :Welcome to the internet"
#define	RPL_UMODEIS			221
#define	RPL_UMODEIS_MSG		"i" // 221 "<user mode string>"
#define RPL_CHANNELMODEIS	324
#define RPL_CHANNELMODEIS_MSG	"" // 324 "<channel> <mode> <mode params>"
#define	RPL_NOTOPIC			331
#define RPL_NOTOPIC_MSG		" :No topic is set" // 331 <channel>
#define RPL_TOPIC			332
#define RPL_TOPIC_MSG		" :" // 332 <channel> <topic>


class	rplManager
{
	typedef std::map<int, std::string> rpl_map;
	typedef std::pair<int, std::string> new_pair;

	private:
		static rplManager	*_rplManager;
		rpl_map	list;

		rplManager();

	public:
		// rplManager(rplManager &copy) { delete _rplManager; }
		// void operator=(const rplManager &other) { delete _rplManager; }
		~rplManager();

		static rplManager	*getInstance();

		std::string	createResponse(int ret, std::string opt_before = "", std::string opt_after = "");
		void		printAll();
};

#endif