#ifndef REPLIES_HPP
	#define REPLIES_HPP

#include "irc_protocole_rules.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include "userClass.hpp"

// ERRORS
#define ERR_NOSUCHNICK				401
#define ERR_NOSUCHNICK_MSG			" :No such nick/channel" // 401 <nickname>
#define ERR_TOOMANYTARGETS			407
#define ERR_TOOMANYTARGETS_MSG		" :Duplicate recipients. No message delivered"
#define ERR_NOTEXTTOSEND			412
#define ERR_NOTEXTTOSEND_MSG		" :No text to send" // 401 <nickname>
#define ERR_CANNOTSENDTOCHAN		404
#define ERR_CANNOTSENDTOCHAN_MSG	" :Cannot send to channel"
#define ERR_NORECIPIENT				411
#define ERR_NORECIPIENT_MSG			" :No recipient given"
#define ERR_ONEUSNICKNAME			432
#define ERR_ONEUSNICKNAME_MSG		" :Erroneous nickname" // 401 <nickname>
#define ERR_NICKNAMEINUSE			433
#define ERR_NICKNAMEINUSE_MSG		" :Nickname is already in use" // 401 <nickname>
#define ERR_NOSUCHCHANNEL			403
#define ERR_NOSUCHCHANNEL_MSG		" :No such channel" // 403 <channel_name>
#define ERR_NOTONCHANNEL			442
#define ERR_NOTONCHANNEL_MSG		" :You're not on that channel" // 442 <channel>
#define ERR_USERONCHANNEL			443
#define ERR_USERONCHANNEL_MSG		" :is already on channel" // 443 <user> <channel>
#define ERR_CHANOPRIVSNEEDED		482
#define ERR_CHANOPRIVSNEEDED_MSG	" :You are not channel operator" // 482 <channel>
#define ERR_NEEDMOREPARAMS			461
#define ERR_NEEDMOREPARAMS_MSG		" :Not enough parameters" // 461 <command>
#define ERR_CHANNELISFULL			471
#define ERR_CHANNELISFULL_MSG		" :Cannot join channel (+l)" // 471 <channel>
#define ERR_INVITEONLYCHAN			473
#define ERR_INVITEONLYCHAN_MSG		" :Cannot join channel (+i)" // 473 <channel>
#define ERR_BADCHANNELKEY			475
#define ERR_BADCHANNELKEY_MSG		" :Cannot join channel (+k)" // 475 <channel>
#define ERR_USERSDONTMATCH			502
#define ERR_USERSDONTMATCH_MSG		" :Cannot change mode for other users"
#define ERR_ALREADYREGISTRED		462
#define ERR_ALREADYREGISTRED_MSG	" :Unauthorized command (already registered)"

// REPLIES
#define RPL_OKNICK					-1
#define RPL_OKNICK_MSG				"NICK "
#define RPL_OKJOIN					-2
#define RPL_OKJOIN_MSG				"JOIN "
#define RPL_OKTOPIC					-3
#define RPL_OKTOPIC_MSG				"TOPIC "
#define RPL_OKPONG					-4
#define RPL_OKPONG_MSG				"PONG "
#define RPL_OKPART					-5
#define RPL_OKPART_MSG				"PART "
#define RPL_OKNAMES					-6
#define RPL_OKNAMES_MSG				"NAMES "
#define RPL_OKNLIST					-7
#define RPL_OKNLIST_MSG				"LIST "
#define RPL_OKNINVITE				-8
#define RPL_OKNINVITE_MSG			"INVITE "
#define RPL_OKPRIVMSG				-9
#define RPL_OKPRIVMSG_MSG			"PRIVMSG "
#define RPL_OKQUIT					-10
#define RPL_OKQUIT_MSG				"QUIT "
#define RPL_OKKICK					-11
#define RPL_OKKICK_MSG				"KICK "
#define RPL_OKMODE					-12
#define RPL_OKMODE_MSG				"MODE "
#define RPL_OKNOTICE				-13
#define RPL_OKNOTICE_MSG			"NOTICE "
#define RPL_OKCONN					001
#define RPL_OKCONN_MSG				" :Welcome to the internet"
#define	RPL_UMODEIS					221
#define	RPL_UMODEIS_MSG				"i" // 221 "<user mode string>"
#define	RPL_LIST					322
#define RPL_LIST_MSG				" :" // 322 <channel> <# visible> :<topic>
#define RPL_LISTEND					323
#define RPL_LISTEND_MSG				":End of LIST" // 323
#define RPL_CHANNELMODEIS			324
#define RPL_CHANNELMODEIS_MSG		" " // 324 "<channel> <mode> <mode params>"
#define	RPL_NOTOPIC					331
#define RPL_NOTOPIC_MSG				" :No topic is set" // 331 <channel>
#define RPL_TOPIC					332
#define RPL_TOPIC_MSG				" :" // 332 <channel> :<topic>
#define RPL_INVITING				341
#define RPL_INVITING_MSG			" " // 341 <channel> <nick>
#define RPL_NAMREPLY				353
#define RPL_NAMREPLY_MSG			"" // 353 "( "=" / "*" / "@" ) <channel> :[ "@" / "+" ] <nick> *( " " [ "@" / "+" ] <nick> )
#define RPL_ENDOFNAMES				366
#define RPL_ENDOFNAMES_MSG			" :End of NAMES list" // 366 <channel> :End of NAMES list


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
		std::string	createResponse(user &usr, int ret, std::string opt_before = "", std::string opt_after = "");
		void		printAll();
};

#endif