#include "ircServer.hpp"
#include "userClass.hpp"

// std::string	getLine(std::string buffer) {
// 	buffer.find()
// }

int		ircServer::readData(clients_vector::iterator client)
{
		int		    maxlen = MSG_MAX_SIZE;
        char	    buff[maxlen];
		int		    recv_ret = 1;
		users_map::iterator	user_x = _users.find(client->fd);
    	size_t      pos = 0;
    	size_t      old = 0;
		std::string	buffer;

		if (user_x != _users.end()) {
			recv_ret = recv(client->fd, buff, maxlen-1, 0);
			if (recv_ret == -1)
				std::cerr << "ERROR recv : " << errno << std::endl;
			else if (recv_ret == 0) {
				user_x->second.setStatus(USER_STATUS_DEL);
				return 0;
			}
			else {
				for (int i = recv_ret; i < maxlen ; i++)
					buff[i] = '\0';
				user_x->second._msg += buff;
			}
			
			if (user_x->second.getStatus() == USER_STATUS_CONNECTED) {
				while (pos != std::string::npos) {
					pos = user_x->second._msg.find("\r\n", old);
					if (pos != std::string::npos) {
						buffer = user_x->second._msg.substr(old, pos - old);
						parse(user_x, buffer);
						user_x->second._msg = user_x->second._msg.substr(pos + 2);
					}
        			old = pos + 1;
				}
			}
		//std::cout << "end reading : " << msg << "[" << msg.length() << "]" << std::endl;
		// this->parse(msg);
		}
		// std::cout << "user_x : " << user_x->first << " " << user_x->second.getId() << std::endl;
		return recv_ret;
}
