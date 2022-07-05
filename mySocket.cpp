#include "mySocket.hpp"


mySocket::mySocket(char *port) : _port(port)//, roger()
{
	this->initAddrInfo();
	this->createMasterSocket();
}

mySocket::~mySocket()
{
	close(_master_sockfd);
	freeaddrinfo(_servinfo);
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
		throw 2;
    }
	// fcntl(_master_sockfd, F_SETFL, O_NONBLOCK);	// disable the capacity to block from accept() recv() etc
	bind_ret = bind(_master_sockfd, _servinfo->ai_addr, _servinfo->ai_addrlen);
	if (bind_ret == -1) {
        std::cerr << "ERROR : Bind" << std::endl;
		close(_master_sockfd);
		freeaddrinfo(_servinfo);
		exit(errno);
    }
}

void	mySocket::startListen()
{
	int	needQuit = 1;

	std::cout << "listening .. " << std::endl;
	listen(_master_sockfd, BACKLOG); // again need to check != 0

	while (1) { // here must be infinit loop
		addr_size = sizeof(their_addr);
		new_fd = accept(_master_sockfd, (struct sockaddr *)&their_addr, &addr_size); // again check // accept will create a new socket to talk with client

		std::cout << "New client #" << new_fd << " from "
				<< inet_ntoa(their_addr.sin_addr) << ":"
				<< ntohs(their_addr.sin_port) << std::endl;

        needQuit = this->readData();
		// create user with new_fd
		//std::cout << "User : " << roger.getName() << " fd : " << roger.getFd() << std::endl; 
		// close(new_fd);
	}
}

int    mySocket::readData()
{
		int		maxlen = 512;   // Semi pif, je crois que c'est la taille max dans le protocol IRC pas sur de devoir le mettre ici
        char	buff[maxlen];	// ne devrait pas etre en local, aura besoin de traitement
		int		recv_ret = 1;
		std::string	msg;

		// recv_ret = recv(new_fd, buff, maxlen-1, 0);
		// if (recv_ret == -1)
		// 	std::cerr << "error : " << errno << std::endl;
		// else if (recv_ret == 0)
		// 	std::cout << "remote host close the connection" << std::endl;
		// else {
		// 	for (int i = recv_ret; i < maxlen ; i++)
		// 		buff[i] = '\0';
		// 	std::cout << "My buffer[" << recv_ret << "] |" << buff << std::endl;
		// }

		while (recv_ret > 0) {
			recv_ret = recv(new_fd, buff, sizeof(buff), 0);
			buff[recv_ret] = '\0';
			std::cout << buff << "|buff size : " << recv_ret << std::endl;
			msg += buff;
		}

		std::cout << "end reading : " << msg << "[" << msg.length() << "]" << std::endl;
		// must be in a sendData function
		std::string res = "Hey Maaaarc !";
		send(new_fd, res.c_str(), res.length(), 0);

		// roger.setFd(new_fd);
		// roger.setName(buf);
		return recv_ret;
}

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
