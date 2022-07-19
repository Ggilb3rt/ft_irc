#include "ircServer.hpp"

/*
	TODO:
	- Finish error management :
		-accept (-1 handled, 0 seems inoperant) but don't want to exit() when -1,
				just print err and go againt, use an arbitrary limit ?
		-send : must create send()function
		-segfault :
			- start fd_killer and ctrl+c before it ends
	- Parsing : 
		- check README to get syntax requirements
		- implement lexer
		- ! multi command in single fd
	- Commands :
		- implement commands in README
	- Miscellaneous :
		?- Remove ip v6 ?
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
		- on DALnet, when try to nick existingName the server let me 60 seconds after that he give me nick GuestXXXX


	TODO: DONE
	- Finish error management :
		-listen: (ok)
		-recv : EAGAIN error check on linux
		- error: when buffer uncomplete the program blocks ;
				sending message from another client put it in waiting list
			==> solution : one buffer by client
	- Channels :
		- when client disconnect : 
			- remove channel if it was the last in
			- change operator if there is no more operator
				==> it works, the first user in the map is set has operator
	-segfault : 
			- scenario :
				- clientX connects, then clientY connects
					clientY disconnects before clientX then clientX disconnects
					(true in branch main)
					==> update _pfds.end() after removeClient()
				- start fd_killer.sh, when process close all sockets, segfault (don't know if problem from fd_killer or us)
					if I close only one socket and use nc => no problems
					==> update iterator in while(ret_poll>0 ...), it was invalidate when removeClient()


*/

ircServer::ircServer(char *port) : _port(port), _nick_suffixe(0)
{
	this->init();

	/* TESTS for TOPIC and rplManager*/
	this->_channel.insert(std::pair<std::string, channel>("lol", channel("lol", 4)));
	std::cout << "Return from TOPIC (not exist) : " << this->topic(4, "lolo") << std::endl;
	std::cout << "Return from TOPIC (geter) : " << this->topic(4, "lol") << std::endl;
	std::cout << "Return from TOPIC (seter): " << this->topic(4, "lol", "New topic for lol") << std::endl;
	std::cout << "Return from TOPIC (not in): " << this->topic(6, "lol", "fouille merde") << std::endl;
	this->join(6, "lol");
	std::cout << "Return from TOPIC (not allowed): " << this->topic(6, "lol", "fouille merde") << std::endl;



	/* SIMULATION test
			- Create x users ( >= 5 )
				- try to create user with same nick
			- each users create or join one or more channels
			- try to change desciption with operator and basic user
			- remove x/2 users
				- try to kick some of them with operator and basic user
	*/
	std::cout << "\n\n===========STARTING SIMULATION TEST============\n\n";

	// create users
	this->printUsers();
	std::cout << "----------------Creating Users----------------\n";
	std::string	names[5] = {"Roger", "Marcel", "Corine", "Corine", "Boby"};
	std::string	nicks[5] = {"Rabbit", "Patoulatchi", "Corine", "Roger", "Toby"};
	for (int i = 15; i < 20; i++) {
		this->addClient(i, nicks[i-15], names[i-15]);
	}
	this->addClient(17);
	this->addClient(20, "Rabbit", "anotherName");
	this->addClient(21, "Rabbit", "anotherName");
	this->printUsers();

	// create/join channels
	this->printChannels();
	std::cout << "----------------Creating and joining some channels----------------\n";
	this->join(_users.find(15)->second.getId(), "ChannelDeRoger");
	this->join(_users.find(16)->second.getId(), "GardienDeLaPaix");
	this->join(_users.find(17)->second.getId(), "GardienDeLaPaix");
	this->join(_users.find(17)->second.getId(), "GardienDeLaPaix");
	this->join(_users.find(19)->second.getId(), "GardienDeLaPaix");
	this->join(_users.find(18)->second.getId(), "Gardien de la paix");
	this->join(_users.find(18)->second.getId(), "ChannelDeRoger");
	this->join(_users.find(18)->second.getId(), "Vive18");
	for (int i = 15; i < 22; i++)
		this->join(_users.find(i)->second.getId(), "Vive18");
	this->printChannels();

	// get and set descriptions
	std::cout << "----------------Getting or Updating descriptions----------------\n";
	std::cout << this->topic(_users.find(18)->second.getId(), "Vive18");
	std::cout << this->topic(_users.find(15)->second.getId(), "ChannelDeRoger", "ViveRoger");
	std::cout << this->topic(_users.find(18)->second.getId(), "ChannelDeRoger");
	std::cout << this->topic(_users.find(18)->second.getId(), "ChannelDeRoger", "ViveMoi");
	std::cout << this->topic(_users.find(19)->second.getId(), "Pouet", "ViveMoi");
	std::cout << std::endl;
	this->printChannels();

	// remove x/2 users
	std::cout << "----------------QUIT/KICK/disconnecting users----------------\n";
	//can't remove client with .removeClient() because I need _pfds
	// only remove client from channels (like in .reomoveClient())
	channel_map::iterator it = _channel.begin();
	channel_map::iterator end = _channel.end();

	while (it !=end){
		it->second.removeUser(17);
		if (it->second.getSize() == 0)
			it = _channel.erase(it);
		else {
			it->second.replaceLastOperator();
			it++;
		}
	}
	it = _channel.begin();
	end = _channel.end();
	while (it !=end) {
		it->second.removeUser(8);
		if (it->second.getSize() == 0)
			it = _channel.erase(it);
		else {
			it->second.replaceLastOperator();
			it++;
		}
	}
	it = _channel.begin();
	end = _channel.end();
	while (it !=end) {
		it->second.removeUser(15);
		if (it->second.getSize() == 0)
			it = _channel.erase(it);
		else {
			it->second.replaceLastOperator();
			it++;
		}
	}
	it = _channel.begin();
	end = _channel.end();
	while (it !=end) {
		it->second.removeUser(18);
		if (it->second.getSize() == 0)
			it = _channel.erase(it);
		else {
			it->second.replaceLastOperator();
			it++;
		}
	}
	this->printChannels();

	it = _channel.begin();
	end = _channel.end();
	while (it !=end) {
		it->second.removeUser(16);
		if (it->second.getSize() == 0)
			it = _channel.erase(it);
		else {
			it->second.replaceLastOperator();
			it++;
		}
	}

	this->printChannels();

	std::cout << "====================================================\n\n";
	// END SIMULATION test
}

ircServer::~ircServer()
{
	close(_master_sockfd);
	freeaddrinfo(_servinfo);
}


