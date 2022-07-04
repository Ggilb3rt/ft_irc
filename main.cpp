#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>

class mySocket
{

#define PORT	"6697" // need to change it with argv[1]
#define BACKLOG 10 // the number of connection allowed on the incomming queue

private:
	struct addrinfo _hints, *_servinfo, *_p;
	char	*_address_name;
	char	_ipstr[INET6_ADDRSTRLEN];
	struct sockaddr_in	their_addr;
	socklen_t	addr_size;

	void	initAddrInfo();

public:
	int		sockfd, new_fd;

	mySocket(char *_address_name);
	~mySocket();

	void	printAddrInfo();
};

mySocket::mySocket(char *address_name) : _address_name(address_name)
{
	this->initAddrInfo();

	// CREATE SOCKET and ADD a PORT
	sockfd = socket(_servinfo->ai_family, _servinfo->ai_socktype, _servinfo->ai_protocol); // need check != 0 and errno
	//fcntl(sockfd, F_SETFL, O_NONBLOCK);	// disable the capacity to block from accept() recv() etc
	bind(sockfd, _servinfo->ai_addr, _servinfo->ai_addrlen); // need check != 0 and errno


	// LISTEN and ACCPET connection
	std::cout << "listening .. " << std::endl;
	listen(sockfd, BACKLOG); // again need to check != 0

	while (1) {
		addr_size = sizeof(their_addr);
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size); // again check // accept will create a new socket to talk with client

		// READ data
		std::cout << "New client #" << new_fd << " from "
				<< inet_ntoa(their_addr.sin_addr) << ":"
				<< ntohs(their_addr.sin_port) << std::endl;

		void	*buf;
		int		maxlen = 512;
		int		recive = 0;

		recive = recv(sockfd, buf, maxlen, 0);
		if (recive == -1)
			std::cerr << "error : " << errno << std::endl;
		else if (recive == 0)
			std::cout << "remote host close the connection" << std::endl;
		else
			std::cout << "My buffer[" << recive << "] |" << buf << "|" << std::endl;
	}
}

mySocket::~mySocket()
{
	close(sockfd);
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

	_addrinfo_status = getaddrinfo(_address_name, PORT, &_hints, &_servinfo);
	if (_addrinfo_status != 0) { // here we need to use the second argument (the port)
		std::cerr << "getaddrinfo error : "  << _addrinfo_status << std::endl;
		throw 1;
	}
}

void	mySocket::printAddrInfo()
{
	std::cout << std::endl << "IP addresses for " << _address_name << " : " << std::endl << std::endl;

	for(_p = _servinfo;_p != NULL; _p = _p->ai_next) {
		void		*addr;
		std::string	ipver;

		// get the pointer to the address itself,
		// different fields in IPv4 and IPv6:
		if (_p->ai_family == AF_INET) { // IPv4
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)_p->ai_addr;
			std::cout << "Port is " << ipv4->sin_port << std::endl;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		} else { // IPv6
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)_p->ai_addr;
			std::cout << "Port is " << ipv6->sin6_port << std::endl;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		// convert the IP to a string and print it:
		inet_ntop(_p->ai_family, addr, _ipstr, sizeof _ipstr);
		std::cout << "\t" << ipver << " : " << _ipstr << std::endl;
	}
}


int main(int argc, char **argv)
{

	if (argc != 2) {
		std::cerr << "usage: showip hostname" << std::endl;
		return 1;
	}

	mySocket	base(argv[1]);
	base.printAddrInfo();
	std::cout << "Fd : " << base.sockfd << std::endl;

	return 0;
}