#include "mySocket.hpp"

/*
	TODO:
	- Finish error management :
		-accept (-1 handled, 0 seems inoperant)
		-recv : EAGAIN error check on linux
		-send : must create send()function
		-listen: TODO
	- Parsing : 
		- check README to get syntax requirements
		- implement lexer
		- ! multi command in single fd
	- Commands :
		- implement commands in README
	- Miscellaneous :
		- Code cleanup
			-------------------
		!! Worry about user suppression
		-> multiple instance of the same user can coexist in
			- channels
			- map in socket object
		-> multiple instance of the same channel can coexist in 
			- users
			- socket object
			-------------------
		POSSIBLE SOLUTION
			create channels with user.fd and access clients with fd
*/

mySocket::mySocket(char *port) : _port(port)//, roger()
{
	this->init();
}

mySocket::~mySocket()
{
	close(_master_sockfd);
	freeaddrinfo(_servinfo);
}

void	mySocket::init()
{
	this->initAddrInfo();
	this->createMasterSocket();
}

void	mySocket::initAddrInfo()
{
	int		_addrinfo_status;
	// INIT STRUCT
	memset(&_hints, 0, sizeof _hints);
	_hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_flags = AI_PASSIVE; // give my local ip

	_addrinfo_status = getaddrinfo(ADDRESS_NAME, _port, &_hints, &_servinfo);
	if (_addrinfo_status != 0) { // here we need to use the second argument (the port)
		std::cerr << "getaddrinfo error " << _addrinfo_status << " : "  << errno << std::endl;
		exit(errno);
	}
}

void	mySocket::createMasterSocket()
{
	int	bind_ret;

	_master_sockfd = socket(_servinfo->ai_family, _servinfo->ai_socktype, _servinfo->ai_protocol);
	if (_master_sockfd == -1) {
        std::cerr << "ERROR : Socket" << std::endl;
		freeaddrinfo(_servinfo);
		exit(errno);
    }
	fcntl(_master_sockfd, F_SETFL, O_NONBLOCK);	// disable the capacity to block from accept() recv() etc // need to check errors
	bind_ret = bind(_master_sockfd, _servinfo->ai_addr, _servinfo->ai_addrlen);
	if (bind_ret == -1) {
        std::cerr << "ERROR : Bind" << std::endl;
		close(_master_sockfd);
		freeaddrinfo(_servinfo);
		exit(errno);
    }
	std::cout << "listening .. " << std::endl;
	listen(_master_sockfd, BACKLOG); // again need to check != 0
}

void	mySocket::startListen()
{
	int	ret_poll = 0;
	int	isStuck = 0;
	struct pollfd	master;
	
	master.fd = _master_sockfd;
	master.events = POLLIN;


	std::vector<struct pollfd>::iterator it = _pfds.begin();
	std::vector<struct pollfd>::iterator end = _pfds.end();


	_pfds.push_back(master);

	while (1) { // here must be infinit loop
		ret_poll = poll(_pfds.data(), _pfds.size(), 0);

		if (ret_poll == -1) {
			// TODO:: Crash Test infinite FD
			std::cerr << errno << std::endl;
			isStuck++;
			if (isStuck > 5)
				exit(1);
		}
		else {
			if (_pfds[0].revents & POLLIN ) {
				struct pollfd	newClient;//!!!!

				addr_size = sizeof(their_addr);
				newClient.fd = accept(_master_sockfd, (struct sockaddr *)&their_addr, &addr_size); // again check 
				if (newClient.fd == -1) {
					std::cerr << errno << std::endl;
				}
				else {
					// accept will create a new socket to talk with client
					newClient.events = MASK;
					_pfds.push_back(newClient);
					_users.insert(std::pair<int, user>(newClient.fd, user(newClient.fd)));
					end = _pfds.end();
					// UNCOMMENT TO PRINT MAP ON USERS
					// for (user_list::iterator it = _users.begin(); it != _users.end(); it++) {
					// 	std::cout << "id == " << it->second.getId() << std::endl;
					// }
				}
			}
			else {
				it = _pfds.begin();
				while (ret_poll > 0 && it != end) {
					ret_poll = handleChange(ret_poll, it);
					it++;
				}
				// Answer to client
			}
		}
		

		// addr_size = sizeof(their_addr);
		// new_fd = accept(_master_sockfd, (struct sockaddr *)&their_addr, &addr_size); // again check // accept will create a new socket to talk with client

		// std::cout << "New client #" << new_fd << " from "
		// 		<< inet_ntoa(their_addr.sin_addr) << ":"
		// 		<< ntohs(their_addr.sin_port) << std::endl;

        // needQuit = this->readData();

		// create user with new_fd
		//std::cout << "User : " << roger.getName() << " fd : " << roger.getFd() << std::endl; 
		// close(new_fd);
	}
}

void	mySocket::removeClient(std::vector<struct pollfd>::iterator it) {
		close(it->fd);
		_users.erase(it->fd);
		_pfds.erase(it);
}

int		mySocket::handleChange(int	ret_poll, std::vector<struct pollfd>::iterator it) {
	if (it->revents & POLLERR) {
		removeClient(it);
		std::cerr << "error: An error has occured" << std::endl;
	}
	else if (it->revents & POLLHUP) {
		removeClient(it);
	}
	else if (it->revents & POLLNVAL) {
		removeClient(it);
		std::cerr << "error: Invalid fd member" << std::endl;
	}
	else if (it->revents & POLLIN) {
		this->readData(it);
		ret_poll--;
	}
	return (ret_poll);
}

int		mySocket::readData(std::vector<struct pollfd>::iterator client)
{
		int		maxlen = 512;   // Semi pif, je crois que c'est la taille max dans le protocol IRC pas sur de devoir le mettre ici
        char	buff[maxlen];	// ne devrait pas etre en local, aura besoin de traitement
		int		recv_ret = 1;
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

// void	mySocket::parse(std::string msg)
// {
// 	// <CMD> [{<PARAM> <SPACE>}]

// 	std::queue<t_lex>	tokens();
// }

void	mySocket::printAddrInfo()
{
   	char    _ipstr[INET6_ADDRSTRLEN];

	std::cout << std::endl << "IP addresses for " << ADDRESS_NAME << " : " << std::endl << std::endl;
	for(_p = _servinfo;_p != NULL; _p = _p->ai_next) {
		void		*addr;
		std::string	ipver;

		// get the pointer to the address itself,
		// different fields in IPv4 and IPv6:
		if (_p->ai_family == AF_INET) { // IPv4
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)_p->ai_addr;
			std::cout << "Port is " << ntohs(ipv4->sin_port) << std::endl;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		} else { // IPv6
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)_p->ai_addr;
			std::cout << "Port is " << ntohs(ipv6->sin6_port) << std::endl;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		// convert the IP to a string and print it:
		inet_ntop(_p->ai_family, addr, _ipstr, sizeof _ipstr);
		std::cout << "\t" << ipver << " : " << _ipstr << std::endl;
	}
}