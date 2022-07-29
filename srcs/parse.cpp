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

    if ((pos = query.find(":")) != std::string::npos) {
        pos += 1;
        longarg = query.substr(pos, query.find(END_MSG, pos) - pos);
        query = query.substr(0, pos - 1);
    }

    std::cout << "QUERY == " << query << std::endl;
    pos = 0;
    
    while (pos != std::string::npos) {
        pos = query.find(' ', old);
        std::cout << "OLD == " << old << " | POS == " << pos << " | SUB == |" << query.substr(old, pos - old) << "|\n";
        argvec.push_back(query.substr(old, pos - old));
        old = pos + 1;
    }

    if (longarg.c_str())
        argvec.push_back(longarg);

    return(handleCommands(it, argvec));
}

bool   ircServer::handleCommands(users_map::iterator &it, std::vector<std::string> &argvec)
{
    if (argvec[0] == "NICK") {
        std::cout << "JE SUIS LAAA\n";
        argvec.erase(argvec.begin());
        return (handleNick(it, argvec));
    }
    // else if (argvec[0] == "LIST") {
        
    // }
    else if (argvec[0] == "PASS") {
        std::cout << "JE SUIS ICIIIIII\n";
        return (checkPass(argvec[1]));
    }
    // else if (argvec[0] == "USER") {
        
    // }
    // else if (argvec[0] == "MODE") {
        
    // }
    // else if (argvec[0] == "PING") {
        
    // }
    // else if (argvec[0] == "TOPIC") {
        
    // }
    // else if (argvec[0] == "JOIN") {
        
    // }
    // else if (argvec[0] == "PART") {
        
    // }
    return (false);
}