#include "ircServer.hpp"

void    ircServer::parse(clients_vector::iterator it, std::string query, int channel)
{
    if (query.length() > MAXLEN_MSG) {
        sendToClient(it->fd, "Message must be less than 510 characters");
    }
    else if (*(query.begin()) == '/') {
        handleCommands(query);
    }
    else {
        sendToChannel(query, channel);
    }
    // check if it starts with /
    //  -> no? send msg, return;
    // check if command exists
    //  handleCmd
}

void    ircServer::handleCommands(clients_vector::iterator it, std::string query)
{
    
}