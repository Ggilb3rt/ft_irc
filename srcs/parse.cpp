#include "ircServer.hpp"

//  TOKENIZE : prefix, cmd, args
//  ->check channel concerned
//  ->check prefix

//  if      ->check command
//              if !->check validity
//                      ->send_msg to channel or nth
//              ->check args
//              ->send cmd(vector(args))
//  else    
//          ->send_msg to channel or nth
bool    ircServer::parse(users_map::iterator &it, std::string query)
{   
    size_t                      pos = 0;
    size_t                      old = 0;
    std::string                 longarg;
    std::vector<std::string>    argvec;

    std::cout << "\n\n--------PARSE--------\n\n" << std::endl;

    std::cout << "QUERY == |" << query << "|\n";
    if ((pos = query.find(":")) != std::string::npos) {
        pos += 1; 
        longarg = query.substr(pos, query.find(END_MSG, pos) - pos);
        query = query.substr(0, pos - 2);
    }

    pos = 0;

    while (pos != std::string::npos) {
        pos = query.find(' ', old);
        if (pos != std::string::npos && query.c_str()[pos - 1] == ',') {
            // TODO: send syntax error
            return (false);
        }
        std::cout << "OLD == " << old << " | POS == " << pos << " | SUB == |" << query.substr(old, pos - old) << "|\n";
        std::string sub = query.substr(old, pos - old);
        argvec.push_back(sub);
        old = pos + 1;
    }

    if (longarg.size()) {
		if (argvec[0] == "USER" && argvec.size() > 5)
			argvec[3] = longarg;
		else
        	argvec.push_back(longarg);
	}
    else {
        if (argvec[0] == "USER") {
            sendToClient(it->first, ERR_NEEDMOREPARAMS, "USER");
            return (false);
        }
    }

    return(handleCommands(it, argvec));
}

#define NB_CMD 14

enum e_commands
{
    NICK,
    PASS,
    USER,
    LIST,
    QUIT,
    MODE,
    PING,
    PRIVMSG,
    NOTICE,
    TOPIC,
    JOIN,
    PART,
	INVITE,
	KICK
};

bool   ircServer::handleCommands(users_map::iterator &it, std::vector<std::string> &argvec)
{
	std::string s_commands[NB_CMD] = {
		"NICK",
		"PASS",
		"USER",
		"LIST",
		"QUIT",
		"MODE",
		"PING",
        "PRIVMSG",
        "NOTICE",
		"TOPIC",
		"JOIN",
		"PART",
		"INVITE",
		"KICK"
	};
	int i = 0;
	for (i = 0; i < NB_CMD; i++)
		if ((argvec[0]) == s_commands[i])
			break ;
    
	switch (i)
	{
		case NICK:
			argvec.erase(argvec.begin());
			return (nick(it, argvec));

		case PASS:
			argvec.erase(argvec.begin());
			return (pass(it, argvec));
		
		case USER:
			argvec.erase(argvec.begin());
			return (handleUser(it, argvec)); 
		
		case LIST:
			argvec.erase(argvec.begin());
			return (list(it, argvec));
			break;

        case PRIVMSG:
			argvec.erase(argvec.begin());
			return (privmsg(it, argvec));
			break;
        
        case NOTICE:
            argvec.erase(argvec.begin());
			return (notice(it, argvec));
			break;
		
		case QUIT:
			argvec.erase(argvec.begin());
			return (quit(it, argvec));

		case MODE:
			argvec.erase(argvec.begin());
			return (mode(it, argvec));

		case PING:
			argvec.erase(argvec.begin());
			return (pong(it, argvec));

		case TOPIC:
			argvec.erase(argvec.begin()); 
			return (topic(it, argvec)); 
		
		case JOIN:
			argvec.erase(argvec.begin());
			return (join(it, argvec));

		case PART:
			argvec.erase(argvec.begin());
			return (part(it, argvec));
		
		case INVITE:
			argvec.erase(argvec.begin());
			return (invite(it, argvec));

		case KICK:
			argvec.erase(argvec.begin());
			return (kick(it, argvec));
		
		default:
			break;
	}

    return (false);
}