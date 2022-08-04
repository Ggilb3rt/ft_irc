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
    // else if (argvec[0] == "QUIT") {
    //     return (quit(argvec));
    // }
    else if (argvec[0] == "USER") {
      argvec.erase(argvec.begin());
      return (handleUser(it, argvec)); 
    }
    // else if (argvec[0] == "MODE") {
    //    argvec.erase(argvec.begin());
    // }
    // else if (argvec[0] == "PING") {
    //    argvec.erase(argvec.begin());    
    // }
    // else if (argvec[0] == "TOPIC") {
    //    argvec.erase(argvec.begin());    
    // }
    else if (argvec[0] == "JOIN") {
       argvec.erase(argvec.begin());
       join(it, argvec);
    }
    // else if (argvec[0] == "PART") {
    //    argvec.erase(argvec.begin());
    //    
    // }
    return (false);
}