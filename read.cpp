#include "ircServer.hpp"

int		ircServer::readData(clients_vector::iterator client)
{
		int		    maxlen = 512;   // Semi pif, je crois que c'est la taille max dans le protocol IRC pas sur de devoir le mettre ici
        char	    buff[maxlen];	// ne devrait pas etre en local, aura besoin de traitement
		int		    recv_ret = 1;
		std::string	msg;

		while (recv_ret > 0) {
			recv_ret = recv(client->fd, buff, sizeof(buff), 0);
			if (recv_ret == 0) {
				removeClient(client);
				std::cout << "client disconnected\n";
			}
			//UNCOMMENT ON LINUX
			// else if (recv_ret == -1) {
			// 
			// 	// send msg to warn client he must leave
			// 	removeClient(client);
			// 	std::cout << strerror(errno) << std::endl; 
			// }
			else if (recv_ret > 0) {
				buff[recv_ret] = '\0';
				msg += buff;
				std::cout << "\n\n---------BUFFER == " << buff << " ---------\n\n";
				if (msg.find("\r\n") != std::string::npos) {
					std::cout << buff << "|buff size : " << recv_ret << std::endl;
					break ;
				}
			}
		}
		std::cout << "end reading : " << msg << "[" << msg.length() << "]" << std::endl;
		// this->parse(msg);

		// msg sent or cmd done
		// must be in a sendData function
		std::string res = "hey you send me :\n" + msg + "!";
		res += END_MSG;
		send(client->fd, res.c_str(), res.length(), 0);
		std::cout << "Send reponse " << res << std::endl;
		// roger.setFd(new_fd);
		// roger.setName(buf);
		return recv_ret;
}
