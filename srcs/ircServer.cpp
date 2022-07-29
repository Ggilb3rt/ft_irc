#include "ircServer.hpp"

/*
	TODO:
	- signals : need it to quit infinite loop (tests no leaks)
	- Finish error management :
		-send : must create send()function
		-segfault :
			- start fd_killer and ctrl+c before it ends
					--> no more segfault but don't remove users, but probleme probably come from tester :
						I try to connect all clients before sending something -> no readData() -> no user_x.setStatus(USER_STATUS_DEL)
						? add POLLRDHUP will help? -> nope
						I change the fd_killer, now he revc after send, it's help to remove all users but still one (if ctr+C between connect() and send())
	- Parsing : 
		- check README to get syntax requirements
		- implement lexer
		- ! multi command in single fd
	- Commands :
		- user_id getUserByName(std::string name);
			--> needed for kick (and other cmds), convertir le nom en id dans le parser
			--> getUserByNick already exist
		- implement commands in README [ ]
	- Miscellaneous :
		?- Remove ip v6 ?
		- Code cleanup
			-------------------
		- on DALnet, when try to nick existingName the server let me 60 seconds after that he give me nick GuestXXXX


	TODO: DONE
	- Finish error management :
		-accept (-1 handled, 0 seems inoperant) but don't want to exit() when -1,
				just print err and go againt, use an arbitrary limit
		-listen: (ok)
		-recv : EAGAIN error check on linux
		- error: when buffer uncomplete the program blocks ;
				sending message from another client put it in waiting list
			==> solution : one buffer by client
		- when client make ctrl+D more than one time he is stucked
			--> probleme from nc ? the rest of the server is ok, sooooo fuck it ?
	- Channels :
		- when client disconnect : 
			- remove channel if it was the last in
			- change operator if there is no more operator
				==> it works, the first user in the map is set has operator
	- Commands :
		- PART and KICK don't remove channel if last user (is it a problem ?)
	- Segfault : 
			- scenario :
				- clientX connects, then clientY connects
					clientY disconnects before clientX then clientX disconnects
					(true in branch main)
					==> update _pfds.end() after removeClient()
				- start fd_killer.sh, when process close all sockets, segfault (don't know if problem from fd_killer or us)
					if I close only one socket and use nc => no problems
					==> update iterator in while(ret_poll>0 ...), it was invalidate when removeClient()
				- wait reach max fd, connect another client; send data from new client, make fd_killer close
					--> can't close all fd before open new one
					xx> again it was an iterator update, the end iterator inside the while(ret_poll>0)
						--> this solution create segfault with basic usage
					==> it works with new version of clients managers
	- Miscellaneous :
		- bit mask system : setter, cleaner, toggler, getter

*/

ircServer::ircServer(char *port) : _port(port), _nick_suffixe(0)
{
	this->init();

	/* TESTS for TOPIC and rplManager*/
	// this->_channel.insert(std::pair<std::string, channel>("lol", channel("lol", 4)));
	// std::cout << "Return from TOPIC (not exist) : " << this->topic(4, "lolo") << std::endl;
	// std::cout << "Return from TOPIC (geter) : " << this->topic(4, "lol") << std::endl;
	// std::cout << "Return from TOPIC (seter): " << this->topic(4, "lol", "New topic for lol") << std::endl;
	// std::cout << "Return from TOPIC (not in): " << this->topic(6, "lol", "fouille merde") << std::endl;
	// this->join(6, "lol");
	// std::cout << "Return from TOPIC (not allowed): " << this->topic(6, "lol", "fouille merde") << std::endl;



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
	this->printUsers();								// empty
	std::cout << "----------------Creating Users----------------\n";
	std::string	names[5] = {"Roger", "Marcel", "Corine", "Corine", "Boby"};
	std::string	nicks[5] = {"Rabbit", "Patoulatchi", "Corine", "Roger", "Toby"};
	for (int i = 15; i < 20; i++) {
		this->addClient(i, nicks[i-15], names[i-15]);
	}
	this->addClient(17);							// error already in use
	this->addClient(20, "Rabbit", "anotherName");	// error rename x
	this->addClient(21, "Rabbit", "anotherName");	// errir rename x+1
	this->printUsers();								// users with id from 15 to 21

	// create/join channels
	this->printChannels();							// empty
	std::cout << "----------------Creating and joining some channels----------------\n";
	this->join(_users.find(15)->second.getId(), "ChannelDeRoger");
	this->join(_users.find(16)->second.getId(), "GardienDeLaPaix");
	this->join(_users.find(17)->second.getId(), "GardienDeLaPaix");
	this->join(_users.find(17)->second.getId(), "GardienDeLaPaix");		// error already in
	this->join(_users.find(19)->second.getId(), "GardienDeLaPaix");
	this->join(_users.find(18)->second.getId(), "Gardien de la paix");	// replace spaces by _
	this->join(_users.find(18)->second.getId(), "ChannelDeRoger");
	this->join(_users.find(18)->second.getId(), "Vive18");
	for (int i = 15; i < 22; i++)
		this->join(_users.find(i)->second.getId(), "Vive18");			// one error with 18 already in
	this->printChannels();												// print all channels with users and functions

	// get and set descriptions
	std::cout << "----------------Getting or Updating descriptions----------------\n";
	this->topic(_users.begin(), std::vector<std::string> ());								// error need more params
	std::vector<std::string>	printVive18(1, "Vive18");
	this->topic(_users.find(18), printVive18);												// print topic
	// std::cout << this->topic(_users.find(18)->second.getId(), "Vive18");						// print topic
	std::vector<std::string>	printChannelDeRoger(1, "ChannelDeRoger");
	std::vector<std::string>	changeChannelDeRoger(printChannelDeRoger);
	std::vector<std::string>	forbidchangeChannelDeRoger(printChannelDeRoger);
	changeChannelDeRoger.push_back("Vive Roger");
	_channel.find("ChannelDeRoger")->second.addFlag(CHAN_MASK_T);
	forbidchangeChannelDeRoger.push_back("ViveMoi");
	this->topic(_users.find(15), changeChannelDeRoger);										// change topic
	this->topic(_users.find(18), printChannelDeRoger);										// print topic
	this->topic(_users.find(18), forbidchangeChannelDeRoger);								// error operator
	this->topic(_users.find(19), std::vector<std::string> (1, "Pouet"));					// error channel not exist
	this->topic(_users.find(19), printChannelDeRoger);										// print topic
	changeChannelDeRoger.pop_back();
	changeChannelDeRoger.push_back("my topic is better");
	changeChannelDeRoger.push_back("must be ignored");
	this->topic(_users.find(15), changeChannelDeRoger);										// change topic

	// OLD WAY 
	// std::cout << this->topic(_users.find(15)->second.getId(), "ChannelDeRoger", "ViveRoger");	// change topic
	// std::cout << this->topic(_users.find(18)->second.getId(), "ChannelDeRoger");				// print topic
	// std::cout << this->topic(_users.find(18)->second.getId(), "ChannelDeRoger", "ViveMoi");		// error operator
	// std::cout << this->topic(_users.find(19)->second.getId(), "Pouet", "ViveMoi");				// error channel not exist
	// std::cout << this->topic(_users.find(19)->second.getId(), "ChannelDeRoger");				// print topic
	std::cout << std::endl;
	this->printChannels();

	// get and set modes (without cmd MODES for now)
	std::cout << "----------------Getting or Updating Channels modes----------------\n";
	_channel.find("Vive18")->second.addFlag(CHAN_MASK_P);     
	_channel.find("Vive18")->second.addFlag(CHAN_MASK_N);
	_channel.find("Vive18")->second.addFlag(CHAN_MASK_N);	// not problems
	_channel.find("Vive18")->second.addFlag(CHAN_MASK_K);
	_channel.find("Vive18")->second.addFlag(CHAN_MASK_O);
	_channel.find("Vive18")->second.removeFlag(CHAN_MASK_P);
	std::cout << "Flags kno are sets, p and i not" << std::endl;
	std::cout << _channel.find("Vive18")->second.isFlagSets(CHAN_MASK_P) << std::endl;	// 0
	std::cout << _channel.find("Vive18")->second.isFlagSets(CHAN_MASK_I) << std::endl;	// 0
	std::cout << _channel.find("Vive18")->second.isFlagSets(CHAN_MASK_K) << std::endl;	// 1
	std::cout << _channel.find("Vive18")->second.isFlagSets(CHAN_MASK_N) << std::endl;	// 1
	std::cout << _channel.find("Vive18")->second.isFlagSets(CHAN_MASK_O) << std::endl;	// 1
	this->printChannels();


	// remove x/2 users
	std::cout << "----------------QUIT/PART/KICK/disconnecting users----------------\n";
	//can't remove client with .removeClient() because I need _pfds
	// only remove client from channels (like in .reomoveClient())

	std::cout << "use QUIT\n";
	this->printUsers();
	this->quit(_users.find(15), std::vector<std::string>());
	this->quit(_users.find(16), std::vector<std::string>());
	this->quit(_users.find(8), std::vector<std::string>());							// not exist ; ignore
	this->quit(_users.find(17), std::vector<std::string>(3, "Tchao les nazes"));
	this->quit(_users.find(18), std::vector<std::string>(1, "Babyee !!"));
	this->printUsers();

	std::cout << "use PART\n";
	this->part(_users.find(18), std::vector<std::string>(1, std::string("ChannelDeRoger"))); // error 403
	this->part(_users.find(83), std::vector<std::string>(1, std::string("ChannelDeRoger"))); // error 403
	this->part(_users.find(4), std::vector<std::string>(1, std::string("GardienDeLaPaix")));	// error 442
	this->part(_users.find(4), std::vector<std::string>(1, std::string("lolilol")));			// error 403
	this->part(_users.find(4), std::vector<std::string>(0, std::string("")));				// error 461
	this->part(_users.find(19), std::vector<std::string>(1, std::string("GardienDeLaPaix")));
	// part with multiples params
	this->join(_users.find(19)->second.getId(), "testPart");
	this->join(_users.find(19)->second.getId(), "testPart2");
	this->join(_users.find(19)->second.getId(), "testPart3");
	this->join(_users.find(19)->second.getId(), "testPart4");
	this->join(_users.find(20)->second.getId(), "testPartNotOn");
	this->printChannels();
	std::vector<std::string> multipart(1, "testPart");
	multipart.push_back("testPart2");
	multipart.push_back("pouet");			// error 403
	multipart.push_back("testPart3");
	multipart.push_back("testPartNotOn");	// error 442
	multipart.push_back("testPart4");
	this->part(_users.find(19), multipart);
	this->printChannels();



	std::cout << "use KICK\n";
	std::vector<std::string> somekicks(1, "Vive18");
	somekicks.push_back("Guest0");	//id 20
	this->kick(_users.find(19), somekicks);
	somekicks[1] = "Toby"; // id 19
	this->kick(_users.find(21), somekicks);			// error ERR_CHANOPRIVSNEEDED
	somekicks[1] = "RandomUser"; // id 55
	this->kick(_users.find(19), somekicks);			// error ERR_NOTONCHANNEL
	somekicks[0] = "banchannel";
	somekicks[1] = "Guest1"; // id 21
	this->kick(_users.find(19), somekicks);			// error ERR_NOSUCHCHANNEL
	somekicks[0] = "Vive18";
	somekicks.push_back("jl'aime pas lui");
	this->kick(_users.find(19), somekicks);
	somekicks.pop_back();
	somekicks[1] = "Toby";
	this->kick(_users.find(19), somekicks);			// auto kick
	
	// old way
	// this->kick("Vive18", 20, 19);
	// this->kick("Vive18", 19, 21); 					// error ERR_CHANOPRIVSNEEDED
	// this->kick("Vive18", 55, 19);					// error ERR_NOTONCHANNEL
	// this->kick("pouet", 21, 19);					// error ERR_NOSUCHCHANNEL
	// this->kick("Vive18", 21, 19, "jl'aime pas");
	// this->kick("Vive18", 19, 19); 					// auto kick possible => oui
	this->printChannels();


	// mode cmd
	std::cout << "----------------MODE cmd now test conversion of flags to mask----------------\n";
	channel_map::iterator modeIt = _channel.begin();
	modeIt->second.convertModeFlagsToMask("qwertyuiopasdfghjklzxcvbnm");
	std::cout << "\t==> " << modeIt->second.convertModeMaskToFlags() << std::endl;
	modeIt->second.convertModeFlagsToMask("-tyuio+pasdfghjk-lzxcvbnm");
	std::cout << "\t==> " << modeIt->second.convertModeMaskToFlags() << std::endl;
	modeIt->second.convertModeFlagsToMask("-tyuio+pasdfghjklzxcvbnm");
	std::cout << "\t==> " << modeIt->second.convertModeMaskToFlags() << std::endl;
	modeIt->second.convertModeFlagsToMask("+o");
	std::cout << "\t==> " << modeIt->second.convertModeMaskToFlags() << std::endl;
	modeIt->second.convertModeFlagsToMask("+o-o");
	std::cout << "\t==> " << modeIt->second.convertModeMaskToFlags() << std::endl;
	modeIt->second.convertModeFlagsToMask("+p-si+mz");
	std::cout << "\t==> " << modeIt->second.convertModeMaskToFlags() << std::endl;
	modeIt->second.convertModeFlagsToMask("+-psi");
	std::cout << "\t==> " << modeIt->second.convertModeMaskToFlags() << std::endl;
	std::cout << "====================================================\n\n";
	// END SIMULATION test

}

ircServer::~ircServer()
{
	close(_master_sockfd);
	freeaddrinfo(_servinfo);
}


