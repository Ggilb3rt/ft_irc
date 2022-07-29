#include "ircServer.hpp"
#include "userClass.hpp"

int		ircServer::readData(clients_vector::iterator client)
{
		int		    maxlen = MSG_MAX_SIZE;
        char	    buff[maxlen];
		int		    recv_ret = 1;
		users_map::iterator	user_x = _users.find(client->fd);

		if (user_x != _users.end()) {
			recv_ret = recv(client->fd, buff, maxlen-1, 0);
			if (recv_ret == -1)
				std::cerr << "ERROR recv : " << errno << std::endl;
			else if (recv_ret == 0) {
				user_x->second.setStatus(USER_STATUS_DEL);
				std::cout << "remote host " << client->fd << " closed the connection" << std::endl;
				return 0;
			}
			else {
				for (int i = recv_ret; i < maxlen ; i++)
					buff[i] = '\0';
				user_x->second._msg += buff;
				std::cout << "My buffer[" << recv_ret << "] in fd["
						<< user_x->second.getId() << "] |"
						<< buff << std::endl;
			}

		//std::cout << "end reading : " << msg << "[" << msg.length() << "]" << std::endl;
		// this->parse(msg);

			if (user_x->second._msg.find(END_MSG) != std::string::npos) {
				// must be in a sendData function
				std::string res = "----hey you send me :\n" + user_x->second._msg + "----!----";
				res += END_MSG;
				user_x->second._msg = "";
				res = "001";
				res += END_MSG;
				send(client->fd, res.c_str(), res.length(), 0);
				std::cout << "Send reponse " << res << std::endl;
			}
		}
		// std::cout << "user_x : " << user_x->first << " " << user_x->second.getId() << std::endl;
		return recv_ret;
}
