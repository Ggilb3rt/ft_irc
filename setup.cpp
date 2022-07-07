#include "ircServer.hpp"

void	ircServer::init()
{
	this->initAddrInfo();
	this->createMasterSocket();
}

void	ircServer::initAddrInfo()
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

void	ircServer::createMasterSocket()
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