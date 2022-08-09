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
        std::cout << "LONGARG == " << longarg << std::endl;
        query = query.substr(0, pos - 2);
        std::cout << "QUERY == |" << query << "|\n";
    } 

    pos = 0;

    while (pos != std::string::npos) {
        pos = query.find(' ', old);
        if (query.c_str()[pos - 1] == ',') {
            // TODO: send syntax error
            return (false);
        }
        std::cout << "OLD == " << old << " | POS == " << pos << " | SUB == |" << query.substr(old, pos - old) << "|\n";
        std::string sub = query.substr(old, pos - old);
        argvec.push_back(sub);
        old = pos + 1;
    }

    if (longarg.size())
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
        size_t i = 0;
        while (i < argvec.size()) {
            std::cout << "ARGVEC[" << i << "] == |" << argvec[i] << "|\n";
            i++;
        }
        argvec.erase(argvec.begin());
        return (nick(it, argvec));
    }
    if (argvec[0] == "PRIVMSG") {
        size_t i = 0;
        while (i < argvec.size()) {
            std::cout << "PRIVMSG[" << i << "] == |" << argvec[i] << "|\n";
            i++;
        }
        argvec.erase(argvec.begin());
        return (privateMsg(it, argvec));
    }
    // else if (argvec[0] == "LIST") {
        
    // }
    else if (argvec[0] == "PASS") {
        std::cout << "JE SUIS ICIIIIII\n";
        argvec.erase(argvec.begin());
        return (pass(it, argvec));
    }
    // else if (argvec[0] == "QUIT") {
    //     return (quit(argvec));
    // }
    else if (argvec[0] == "USER") {
          size_t i = 0;
      argvec.erase(argvec.begin());
        while (i < argvec.size()) {
            std::cout << "ARGVEC[" << i << "] == " << argvec[i] << std::endl;
            i++;
        }
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
           size_t i = 0;
        while (i < argvec.size()) {
            std::cout << "ARGVEC[" << i << "] == " << argvec[i] << std::endl;
            i++;
        }
       argvec.erase(argvec.begin());
       join(it, argvec);
    }
    // else if (argvec[0] == "PART") {
    //    argvec.erase(argvec.begin());
    //    
    // }
    return (false);
}