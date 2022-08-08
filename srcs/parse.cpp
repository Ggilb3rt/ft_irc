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

    std::cout << "QUERY == " << query << std::endl;
    if ((pos = query.find(":")) != std::string::npos) {
        pos += 1;
        longarg = query.substr(pos, query.find(END_MSG, pos) - pos);
        query = query.substr(0, pos - 1);
    }

    
    pos = 0;

    while (pos != std::string::npos) {
        pos = query.find(' ', old);
        if (query.c_str()[pos - 1] == ',') {
            // TODO: send syntax error
            return (false);
        }
        std::cout << "OLD == " << old << " | POS == " << pos << " | SUB == |" << query.substr(old, pos - old) << "|\n";
        argvec.push_back(query.substr(old, pos - old));
        old = pos + 1;
    }

    if (longarg.c_str())
        argvec.push_back(longarg);
    // else {
    //     if (argvec[0] == "USER") {
    //         // send err: no double dot before real name
    //         return (false);
    //     }
    // }

    return(handleCommands(it, argvec));
}

//? Don't forget to change this value if you modify the number of commands below
# define NB_CMD 10

enum e_commands
{
	NICK,
	LIST,
	PASS,
	QUIT,
	USER,
	MODE,
	PING,
	TOPIC,
	JOIN,
	PART
};

bool   ircServer::handleCommands(users_map::iterator &it, std::vector<std::string> &argvec)
{
	std::string s_commands[NB_CMD] = {
		"NICK",
		"LIST",
		"PASS",
		"QUIT",
		"USER",
		"MODE",
		"PING",
		"TOPIC",
		"JOIN",
		"PART"
	};
	int i = 0;
	for (i = 0; i < NB_CMD; i++)
		if ((argvec[1]) == s_commands[i])
			break ;
    
	switch (i)
	{
		case NICK:
			std::cout << "JE SUIS LAAA\n";
			argvec.erase(argvec.begin());
			return (handleNick(it, argvec));
		
		case LIST:
			break;
		
		case PASS:
			std::cout << "JE SUIS ICIIIIII\n";
			return (checkPass(argvec[1]));
		
		case USER:
			argvec.erase(argvec.begin());
			return (handleUser(it, argvec)); 
		
		case MODE:
			// argvec.erase(argvec.begin());
			break;

		case PING:
			// argvec.erase(argvec.begin());
			break;

		case TOPIC:
			// argvec.erase(argvec.begin());  
			break;
		
		case JOIN:
			argvec.erase(argvec.begin());
			join(it, argvec);
			break;

		case PART:
			argvec.erase(argvec.begin());
			break;
		
		default:
			break;
	}
    return (false);
}