# ft_irc
My own irc server

## Socket
A socket is a way to speak to other programs using Unix file descriptors.
It creates a connection between two programms.
There is many type of internet sockets but we will used "Stream sockets" because it's a two way connected communication.

socket = ip address + port nb
ex : 127.0.0.1:80

Qt of port = (2^16)-1, so 0-65535 [source]https://fr.wikipedia.org/wiki/Port_(logiciel)
All blocs of data find the servers with his ip address, then the server sends each blocs to the good process with the port number.


```c
// This code is for client, not for server and he don't listen the response ==> this is useless

#define MAX_SIZE 512

int                 socketfd;
struct sockaddr     serveradd;
char                buff[MAX_SIZE];
size_t              bufflen;

bzero(buff, MAXSIZE);
// Prepare and connect to socket
socketfd = socket(AF_INET, SOCK_STREAM, 0);  // AF_INET == use ipv4, SOCK_STREAM == can write and read, 0 == TCP
if (socketfd < 0)
  err_stop();
serveradd.sin_family = AF_INET;
serveradd.sin_port = htons(SERVER_PORT);

if (connect(socketfd, &serveradd, sizeof(serveradd)) < 0)
  err_connect();

//Prepare and send msg
buff = "SALUT !!\r\n" // I know this will not works
bufflen = strlen(buff);

if (send(socketfd, &buff, bufflen, MSG_OOB) != bufflen) // MSG_OOB == out of band data, don't know what it is
  err_sending();

```

## Client
### Irssi
[doc] https://irssi.org/documentation/
Install on Linux
```bash
sudo apt install irssi
```
Install in 42
```bash
brew install irssi
```

## Search
### Questions
Does we need to implement :
- services

### Server
Name maximum length 63 characters (RFC2812 1.1)

### Channels
> "A channel is a named group of one or more users which will all
> receive messages addressed to that channel.  A channel is
> characterized by its name, properties and current members."

Name maximum length 50 char beginning with '&', '#', '+', or '!', without spaces ' ', a control G (ASCII 7) or a comma ','
Channel names are case insensitive. (RFC2812 1.3)

Standard channels ('&', '#', '+') are implicitly created when first user joins it and die when the last user leaves it. (RFC2811 3.1)
Safe channels ('!') are not implicitly created. The user must request the creation using JOIN with channel id replace by '!'. The name of this channel is the concatenation of 5 characters + x characters (user chooses). The user become "channel creator". Theses channels may continue to exist after the last user left.

Cf Creator to see special habilities of them
Cf Operators to see special habilities of them

#### Channel Modes
For users :
  -O, -o, -V,
For channels :
  -a, -i, -m, -n, -q, -p, -s, -r, -t, -k, -l, -b, -e, -I (see details in RFC2811 4.)
All these modes can be manipulated by "channel operators" by using MODE command

### User
Each user is distinguished from other by a unique nickname, max length 9 characters (always real ?) (RFC2812 1.2.1)

#### Operators
They are clients with specials habilities (it's looks like administrors)

Channel operator (chop or chanop) is the "owner" of that channel. The '@' is next to his nickname
He can :
- KICK (RFC1459 4.2.8)
- MODE (RFC1459 4.2.3)
- INVITE (RFC1459 4.2.7) (mode +i (RFC2811 2.4))
- TOPIC (RFC1459 4.2.4)
Not needed :
- SQUIT (RFC2812 3.1.8) because no server-server communication
(cf RFC1459 1.3.1)

The granting of operator privilegs need some requirements (cf RFC1459 8.12.2)

#### Creator
A user who create a channel with the character '!' as prefix is the "channel creator" (-O). Upon creation of the channel,
this user is also given channel operator status. (RFC2811 2.4.2)

### Messages
- 3 mains parts : the prefix (optional), the command and the command params (up to 15). They are separated by unless one space (ASCII 0x20). (cf RFC1459 2.3)
- IRC messages are always lines of characters (512 max) with a CR-LF (Carriage Return - Line Feed) (\r\n) pair. (cf RFC1459 2.3) Not all implementations use CR-LF (cf RFC1459 8.)
- see pseudo messages in RFC1459 2.3.1

Needed commands (cf RFC1459 4.x) :
We have to debate about what is needed and what is not following the subject.
- Connection (4.1)
  - (PASS ??)
  - NICK
  - USER
  - SERVER (not needed, only server-server)
  - OPER
  - QUIT
  - SQUIT (not needed, only server-server or alias of QUIT)
- Channel (4.2)
  - JOIN
  - PART
  - MODE
  - TOPIC
  - NAME
  - LIST
  - INVITE
  - KICK
- Server (4.3) (only with 'this' because no server-server communication)
  - VERSION
  - STATS
  - LINKS (not needed)
  - TIME
  - CONNECT (not needed)
  - TRACE (not needed)
  - ADMIN
  - INFO
- Sending messages (4.4)
  - PRIVMSG (mode +n, +m, +v (RFC2811 2.4))
  - NOTICE
- User (4.5)
  - WHO
  - WHOIS
  - WHOWAS
- Miscellaneous messages (4.6)
  - KILL
  - PING
  - PONG (not needed because only for client or server-server(?))
  - ERROR
- OPTIONALS
  - lol nope
- plus all operators specials habilities

### Error replies
cf RFC1459 section 6, we just have to copie paste all in a file

### Funny things
> "Because of IRC's scandanavian origin, the characters {}| are
> considered to be the lower case equivalents of the characters []\,
> respectively. This is a critical issue when determining the
> equivalence of two nicknames." (cf RFC1459 2.2)

## Sources
[client] https://irssi.org/New-users/
[socket_prog] https://beej.us/guide/bgnet/html

[before_RFC] http://chi.cs.uchicago.edu/chirc/intro.html
[RFC1459:Base] https://datatracker.ietf.org/doc/html/rfc1459
[RFC2810:Architecture] https://datatracker.ietf.org/doc/html/rfc2810
[RFC2811:ChanneManagement] https://datatracker.ietf.org/doc/html/rfc2811
[RFC2812:ClientProtocole] https://datatracker.ietf.org/doc/html/rfc2812
[RFC2813:ServerProtocole] https://datatracker.ietf.org/doc/html/rfc2813
[RFC7194:TLS/SSL] https://www.rfcreader.com/#rfc7194