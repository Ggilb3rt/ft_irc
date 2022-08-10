#include "ircServer.hpp"

/*
	TODO:
	- VALGRIND errors :
		- Parsing
			- invalid read on USER cmd
		- read.cpp
			- invalid read of 2 bytes
		- many errors when use part cmd
	- Finish error management :
		-segfault :
			- start fd_killer and ctrl+c before it ends
					--> no more segfault but don't remove users, but probleme probably come from tester :
						I try to connect all clients before sending something -> no readData() -> no user_x.setStatus(USER_STATUS_DEL)
						? add POLLRDHUP will help? -> nope
						I change the fd_killer, now he revc after send, it's help to remove all users but still one (if ctr+C between connect() and send())
	- Parsing : 
	- Commands :
	- Miscellaneous :
		?- can i use USER cmd to show my user infos ?
		?- Remove ip v6 ?
		- Code cleanup
			-------------------
		- on DALnet, when try to nick existingName the server let me 60 seconds after that he give me nick GuestXXXX


	TODO: DONE
	- signals : need it to quit infinite loop (tests no leaks)
	- Finish error management :
		-send : must create send()function
		-accept (-1 handled, 0 seems inoperant) but don't want to exit() when -1,
				just print err and go againt, use an arbitrary limit
		-listen: (ok)
		-recv : EAGAIN error check on linux
		- error: when buffer uncomplete the program blocks ;
				sending message from another client put it in waiting list
			==> solution : one buffer by client
		- when client make ctrl+D more than one time he is stucked
			--> probleme from nc ? the rest of the server is ok, sooooo fuck it ?
	- Parsing : 
		- check README to get syntax requirements
		- implement lexer
		- ! multi command in single fd
	- Channels :
		- when client disconnect : 
			- remove channel if it was the last in
			- change operator if there is no more operator
				==> it works, the first user in the map is set has operator
	- Commands :
		- PART and KICK don't remove channel if last user (is it a problem ?)
		- user_id getUserByName(std::string name);
			--> needed for kick (and other cmds), convertir le nom en id dans le parser
			--> getUserByNick already exist
		- implement commands in README [ ]
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

ircServer::ircServer(char *port, std::string pass) : _port(port), _nick_suffixe(0), _pass(pass)
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
	
	std::cout << "\n\n===========STARTING SIMULATION TEST============\n\n";

	// create users
	this->printUsers();								// empty
	std::cout << "----------------Creating Users----------------\n";
	std::string	names[5] = {"Roger", "Marcel", "Corine", "Corine", "Bobby McFerrin"};
	std::string	nicks[5] = {"Rabbit", "Patoulatchi", "Corine", "Roger2", "Toby"};
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
	this->join(_users.find(15), std::vector<std::string>(1, "ChannelDeRoger"));
	this->join(_users.find(16), std::vector<std::string>(1, "GardienDeLaPaix"));
	this->join(_users.find(17), std::vector<std::string>(1, "GardienDeLaPaix,GardienDeLaPaix")); // error already in
	this->join(_users.find(19), std::vector<std::string>(1, "GardienDeLaPaix"));
	this->join(_users.find(18), std::vector<std::string>(1, "Gardien de la paix,ChannelDeRoger,Vive18"));	// replace spaces by _
	for (int i = 15; i < 22; i++)
		this->join(_users.find(i), std::vector<std::string>(1, "Vive18"));			// one error with 18 already in
	this->join(_users.find(15), std::vector<std::string>());		// error ERR_NEEDMOREPARAMS
	this->printChannels();					// print all channels with users and functions
	std::vector<std::string>	need_more_key(1, "1,2,3,4");
	need_more_key.push_back("1,2,3");
	this->join(_users.find(15), need_more_key);						// error ERR_NEEDMOREPARAMS
	//! NEED TO CHANGE THIS TESTS WHEN MODE WILL BE OK
	channel_map::iterator little_mode = _channel.begin();
	std::vector<std::string>	modeMoove(1, little_mode->first);
	modeMoove.push_back("i");
	this->mode(_users.find(15), modeMoove);
	// little_mode->second.convertModeFlagsToMask("i");
	this->join(_users.find(21), std::vector<std::string>(1, "ChannelDeRoger"));	// error ERR_INVITEONLYCHAN
	modeMoove[1] = "-i+l";
	modeMoove.push_back("limit");
	this->mode(_users.find(15), modeMoove);
	// little_mode->second.convertModeFlagsToMask("-i+l");
	std::cout << "user limit " << little_mode->second.getUserLimit() << ", users in " << little_mode->second.getSize() << std::endl;
	little_mode->second.setUserLimit(3);
	this->join(_users.find(21), std::vector<std::string>(1, "ChannelDeRoger"));
	this->join(_users.find(20), std::vector<std::string>(1, "ChannelDeRoger"));	// error ERR_CHANNELISFULL
	std::cout << "user limit " << little_mode->second.getUserLimit() << ", users in " << little_mode->second.getSize() << std::endl;
	modeMoove[1] = "-l+k";
	this->mode(_users.find(15), modeMoove);
	// little_mode->second.convertModeFlagsToMask("-l+k");
	little_mode->second.setPassword("boom baby");
	std::vector<std::string>	try_key(1, "ChannelDeRoger");
	try_key.push_back("pouet");
	this->join(_users.find(20), std::vector<std::string>(1, "ChannelDeRoger"));	// error ERR_NEEDMOREPARAMS
	this->join(_users.find(20), try_key);										// error ERR_BADCHANNELKEY
	try_key[1] = "boom baby";
	this->join(_users.find(20), try_key);


	this->printChannels();




	// list
	std::cout << "----------------CMD LIST----------------\n";
	this->list(_users.find(20), std::vector<std::string>(0, ""));
	std::cout << "\nvector with one empty el\n";
	this->list(_users.find(20), std::vector<std::string>(1, ""));
	std::cout << "\nprivate chan, user in\n";
	modeMoove[1] = "-k+p";
	this->mode(_users.find(15), modeMoove);
	// little_mode->second.convertModeFlagsToMask("+p");
	this->list(_users.find(20), std::vector<std::string>(0, ""));	// print chan without topic
	std::cout << "\nprivate chan, user out\n";
	this->part(_users.find(20), std::vector<std::string>(1, "ChannelDeRoger"));
	this->list(_users.find(20), std::vector<std::string>(0, ""));	// don't print ChannelDeRoger
	
	std::cout << "\nsecret chan, user out\n";
	modeMoove[1] = "-p+s";
	this->mode(_users.find(15), modeMoove);
	// little_mode->second.convertModeFlagsToMask("-p+s");
	this->list(_users.find(20), std::vector<std::string>(0, ""));	// don't print ChannelDeRoger
	std::cout << "\njoin back chan\n";
	this->join(_users.find(20), std::vector<std::string>(1, "ChannelDeRoger"));
	std::cout << "\nsecret chan, user in\n";
	this->list(_users.find(20), std::vector<std::string>(0, ""));

	std::cout << "\nlist a list\n";
	this->list(_users.find(20), std::vector<std::string>(1, ""));								// end
	this->list(_users.find(20), std::vector<std::string>(1, "Gardien_de_la_paix"));				// print 1
	this->list(_users.find(20), std::vector<std::string>(1, "Vive18,ChannelDeRoger"));			// print 2
	this->list(_users.find(20), std::vector<std::string>(1, "Vive18,ChannelDeRoger,Pouet"));	// print 2
	std::vector<std::string>	list_some(1, "Gardien_de_la_paix");
	list_some.push_back("Vive18");
	list_some.push_back("Vive18");
	this->list(_users.find(20), list_some);														// print 1


	// names
	std::cout << "\n\n----------------CMD NAMES----------------\n";
	this->names(_users.find(20), std::vector<std::string>(0, ""));							// print all
	std::cout << "\nvector with one empty el\n";
	this->names(_users.find(20), std::vector<std::string>(1, ""));							// print nothing
	std::cout << "\nbasics\n";
	this->names(_users.find(20), std::vector<std::string>(4, "ChannelDeRoger"));
	this->names(_users.find(20), std::vector<std::string>(1, "Vive18,Gardien_de_la_paix"));
	std::cout << "\nsecret chan, user out\n";
	this->names(_users.find(19), std::vector<std::string>(1, "ChannelDeRoger"));
	std::cout << "\nprivate chan, user out\n";
	modeMoove[1] = "+p-s";
	this->mode(_users.find(15), modeMoove);
	// little_mode->second.convertModeFlagsToMask("+p");
	this->names(_users.find(19), std::vector<std::string>(1, "ChannelDeRoger"));
	little_mode++;
	modeMoove[1] = "+p";
	this->mode(_users.find(16), modeMoove);
	// little_mode->second.convertModeFlagsToMask("+p");
	std::cout << "\nall chan with some private chan, other not\n";
	this->names(_users.find(20), std::vector<std::string>(0, ""));							// print private when is in, not when out


	// invite
	std::cout << "\n\n----------------CMD INVITE----------------\n";
	std::vector<std::string>	invite1(1, "Roger2");
	invite1.push_back("Vive18");
	this->invite(_users.find(20), std::vector<std::string>(0, ""));				// 461 ERR_NEEDMOREPARAMS
	this->invite(_users.find(20), std::vector<std::string>(1, ""));				// 461 ERR_NEEDMOREPARAMS
	this->invite(_users.find(20), std::vector<std::string>(2, ""));				// 401 ERR_NOSUCHNICK
	this->invite(_users.find(20), std::vector<std::string>(2, "UserRandom"));	// 401 ERR_NOSUCHNICK
	std::cout << this->invite(_users.find(20), std::vector<std::string>(2, "Roger2")) << std::endl;		// nothing (chan does not exist)
	this->invite(_users.find(20), invite1);										// 443 ERR_USERONCHANNEL
	invite1[1] = "Gardien_de_la_paix";
	this->invite(_users.find(20), invite1);										// 442 ERR_NOTONCHANNEL
	invite1[1] = "GardienDeLaPaix";
	this->invite(_users.find(19), invite1);										// 341 RPL_INVITING
	modeMoove[1] = "+i-p";
	this->mode(_users.find(16), modeMoove);
	// little_mode->second.convertModeFlagsToMask("+i");
	this->invite(_users.find(19), invite1);										// 482 ERR_CHANOPRIVSNEEDED


	// mode cmd
	std::cout << "----------------MODE cmd now test conversion of flags to mask----------------\n";
	this->mode(_users.find(20), std::vector<std::string>(0, ""));						//	ERR_NEEDMOREPARAMS
	this->mode(_users.find(20), std::vector<std::string>(1, ""));						//	ERR_NEEDMOREPARAMS
	this->mode(_users.find(20), std::vector<std::string>(2, ""));						//	ERR_NOSUCHCHANNEL
	this->mode(_users.find(20), std::vector<std::string>(2, "pouet"));					//	pouet ERR_NOSUCHCHANNEL
	this->mode(_users.find(20), std::vector<std::string>(2, "Gardien_de_la_paix"));		//	ERR_NOTONCHANNEL
	std::vector<std::string>	all_you_need_is_love(1, "Vive18");
	all_you_need_is_love.push_back("t");
	this->mode(_users.find(20), all_you_need_is_love);									// t
	all_you_need_is_love.push_back("7limitOfLOL");
	all_you_need_is_love.push_back("Roger2");
	all_you_need_is_love.push_back("passSoCOOL");
	all_you_need_is_love[1] = "qwertyuiopasdfghjklzxcvbnm1234567890!@#$%&*()^_=}{[]|\\:;,.></?\'\"`~";
	this->mode(_users.find(18), all_you_need_is_love);									// opsitnmlbvk
	std::cout << "channel password " << this->_channel.find("Vive18")->second.getPassword() << std::endl;	// passSoCOOL
	all_you_need_is_love[1] = "-tyui+pasdfghjk-lzxcvbnm";
	this->mode(_users.find(20), all_you_need_is_love);					// psk
	all_you_need_is_love[1] = "+o";
	this->mode(_users.find(18), all_you_need_is_love);					// ERR_NOSUCHNICK
	all_you_need_is_love[1] = "+p-si+mz";
	this->mode(_users.find(20), all_you_need_is_love);					// pmk
	all_you_need_is_love[1] = "+-psi";
	this->mode(_users.find(20), all_you_need_is_love);					// mk
	std::cout << "channel password " << this->_channel.find("Vive18")->second.getPassword() << std::endl;	// 7limitOfLOL
	all_you_need_is_love[1] = "+-mkipiwrl";
	this->mode(_users.find(20), all_you_need_is_love);					// empty
	std::cout << "channel password " << this->_channel.find("Vive18")->second.getPassword() << std::endl;	// ""
	all_you_need_is_love[1] = "+io";
	this->mode(_users.find(20), all_you_need_is_love);					// ERR_CHANOPRIVSNEEDED
	all_you_need_is_love[1] = "+k";
	this->mode(_users.find(20), all_you_need_is_love);					// k
	all_you_need_is_love[1] = "+l";
	this->mode(_users.find(20), all_you_need_is_love);					// lk
	std::cout << "channel limit " << this->_channel.find("Vive18")->second.getUserLimit() << std::endl;	// 7
	all_you_need_is_love[1] = "-ilk";
	this->mode(_users.find(20), all_you_need_is_love);					// empty
	std::cout << "channel limit " << this->_channel.find("Vive18")->second.getUserLimit() << std::endl;	// 0
	all_you_need_is_love[1] = "+kl";
	this->mode(_users.find(20), all_you_need_is_love);					// lk
	this->printChannels();
	std::cout << "\nfocus on +-o mode\n";
	std::vector<std::string>	love_love(1, "Vive18");
	love_love.push_back("-lk");
	this->mode(_users.find(18), love_love);								// 324 empty
	love_love.push_back("Patoulatchi");
	love_love[1] = "+o";
	this->mode(_users.find(15), love_love);								// ERR_CHANOPRIVSNEEDED
	this->mode(_users.find(18), love_love);								// 324 empty; 16 is operator
	love_love[1] = "-o";
	this->mode(_users.find(18), love_love);								// ERR_USERSDONTMATCH
	this->mode(_users.find(16), love_love);								// 324 empty; 16 no more operator


	// get and set descriptions
	std::cout << "----------------Getting or Updating descriptions----------------\n";
	this->topic(_users.begin(), std::vector<std::string> ());								// error need more params
	std::vector<std::string>	printVive18(1, "Vive18");
	this->topic(_users.find(18), printVive18);												// print topic
	std::vector<std::string>	printChannelDeRoger(1, "ChannelDeRoger");
	std::vector<std::string>	changeChannelDeRoger(printChannelDeRoger);
	std::vector<std::string>	forbidchangeChannelDeRoger(printChannelDeRoger);
	changeChannelDeRoger.push_back("Vive Roger");
	std::vector<std::string>	add_mode_t(1, "ChannelDeRoger");
	add_mode_t.push_back("+t");
	this->mode(_users.find(15), add_mode_t);												// print 324 pt
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

	std::cout << std::endl;
	this->printChannels();

	// // remove x/2 users
	// std::cout << "----------------QUIT/PART/KICK/disconnecting users----------------\n";
	// //can't remove client with .removeClient() because I need _pfds
	// // only remove client from channels (like in .reomoveClient())

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
	this->join(_users.find(19), std::vector<std::string>(1, "testPart"));
	this->join(_users.find(19), std::vector<std::string>(1, "testPart2"));
	this->join(_users.find(19), std::vector<std::string>(1, "testPart3"));
	this->join(_users.find(19), std::vector<std::string>(1, "testPart4"));
	this->join(_users.find(20), std::vector<std::string>(1, "testPartNotOn"));
	// this->join(_users.find(19)->second.getId(), "testPart");
	// this->join(_users.find(19)->second.getId(), "testPart2");
	// this->join(_users.find(19)->second.getId(), "testPart3");
	// this->join(_users.find(19)->second.getId(), "testPart4");
	// this->join(_users.find(20)->second.getId(), "testPartNotOn");
	this->printChannels();
	std::vector<std::string> multipart(1, "testPart");
	multipart.push_back("testPart2");
	multipart.push_back("pouet");			// error 403
	multipart.push_back("testPart3");
	multipart.push_back("testPartNotOn");	// error 442
	multipart.push_back("testPart4");
	this->part(_users.find(19), multipart);
	this->printChannels();



	// std::cout << "use KICK\n";
	// std::vector<std::string> somekicks(1, "Vive18");
	// somekicks.push_back("Guest0");	//id 20
	// this->kick(_users.find(19), somekicks);
	// somekicks[1] = "Toby"; // id 19
	// this->kick(_users.find(21), somekicks);			// error ERR_CHANOPRIVSNEEDED
	// somekicks[1] = "RandomUser"; // id 55
	// this->kick(_users.find(19), somekicks);			// error ERR_NOTONCHANNEL
	// somekicks[0] = "banchannel";
	// somekicks[1] = "Guest1"; // id 21
	// this->kick(_users.find(19), somekicks);			// error ERR_NOSUCHCHANNEL
	// somekicks[0] = "Vive18";
	// somekicks.push_back("jl'aime pas lui");
	// this->kick(_users.find(19), somekicks);
	// somekicks.pop_back();
	// somekicks[1] = "Toby";
	// this->kick(_users.find(19), somekicks);			// auto kick
	
	// // old way
	// // this->kick("Vive18", 20, 19);
	// // this->kick("Vive18", 19, 21); 					// error ERR_CHANOPRIVSNEEDED
	// // this->kick("Vive18", 55, 19);					// error ERR_NOTONCHANNEL
	// // this->kick("pouet", 21, 19);					// error ERR_NOSUCHCHANNEL
	// // this->kick("Vive18", 21, 19, "jl'aime pas");
	// // this->kick("Vive18", 19, 19); 					// auto kick possible => oui
	// this->printChannels();


	std::cout << "====================================================\n\n";

	// this->printChannels();

	// END SIMULATION test
*/
}

ircServer::~ircServer()
{
	close(_master_sockfd);
	freeaddrinfo(_servinfo);
}


