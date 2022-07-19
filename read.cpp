#include "ircServer.hpp"
#include "userClass.hpp"

int		ircServer::readData(clients_vector::iterator client)
{
		int		    maxlen = MSG_MAX_SIZE;
        char	    buff[maxlen];
		int		    recv_ret = 1;
		users_map::iterator	user_x = _users.find(client->fd);


		recv_ret = recv(client->fd, buff, maxlen-1, 0);
		if (recv_ret == -1)
			std::cerr << "ERROR recv : " << errno << std::endl;
		// else if (recv_ret == 0)
		// 	std::cout << "remote host close the connection" << std::endl;
		else {
			for (int i = recv_ret; i < maxlen ; i++)
				buff[i] = '\0';
			user_x->second._msg += buff;
			// std::cout << "My buffer[" << recv_ret << "] in fd["
			// 		<< user_x->second.getId() << "] |"
			// 		<< buff << std::endl;
		}

		// while (recv_ret > 0) {
		// 	recv_ret = recv(client->fd, buff, sizeof(buff), 0);
		// 	if (recv_ret == 0) {
		// 		removeClient(client);
		// 		std::cout << "client disconnected\n";
		// 	}
		// 	//UNCOMMENT ON LINUX
		// 	// else if (recv_ret == -1) {
		// 	// 
		// 	// 	// send msg to warn client he must leave
		// 	// 	removeClient(client);
		// 	// 	std::cout << strerror(errno) << std::endl; 
		// 	// }
		// 	else if (recv_ret > 0) {
		// 		buff[recv_ret] = '\0';
		// 		msg += buff;
		// 		std::cout << "\n\n---------BUFFER == " << buff << " ---------\n\n";
		// 		if (msg.find("\n") != std::string::npos) { //! find must search END_MSG
		// 			std::cout << buff << "|buff size : " << recv_ret << std::endl;
		// 			break ;
		// 		}
		// 	}
		// }
		//std::cout << "end reading : " << msg << "[" << msg.length() << "]" << std::endl;
		// this->parse(msg);

 		if (user_x->second._msg.find(END_MSG) != std::string::npos) { //! find must search END_MSG
			// must be in a sendData function
			std::string res = "----hey you send me :\n" + user_x->second._msg + "----!----";
			res += END_MSG;
			user_x->second._msg = "";
			send(client->fd, res.c_str(), res.length(), 0);
			std::cout << "Send reponse " << res << std::endl;
		}
		return recv_ret;
}
