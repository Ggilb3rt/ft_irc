#include "ircServer.hpp"


// considering the input is an iterator->[client_fd][user], and an already parsed string nick

void    ircServer::handleNick(users_map::iterator pair, std::string newNick) {
    users_map::iterator beg = _users.begin();
    users_map::iterator end = _users.end();
    std::string         res;

    while (beg != end) {
        if (beg->second.getNick() == newNick) {
            res =  "Nickname already exists !";
            sendToClient(pair->first, res);
            newNick += "_";
        }
        beg++;
    }
    // if above 9 ask for another nickname ? or disconnect ?
    if (newNick.length() > 9) {
        res = "Nickname must be less than 10 characters !";
        sendToClient(pair->first, res);
    }
    else {
        res = "Nickname changed to :" + newNick;
        pair->second.setNick(newNick);
        sendToClient(pair->first, res);
    }
}
