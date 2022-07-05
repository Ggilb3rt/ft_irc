![alt text](https://i.kym-cdn.com/entries/icons/facebook/000/006/877/707538ef3afa883c1d146b42cf01bac2.jpg "welcome to the internet")

# ft_irc
Create an irc server following the RFC1459

## Socket
A socket is a way to speak to other programs using Unix file descriptors.
It creates a connection between two programms.
There is many type of internet sockets but we will used "Stream sockets" because it's a two way connected communication.

> socket = ip address + port nb
> ex : 127.0.0.1:80

Qt of port = (2^16)-1, so 0-65535[^1]


All blocs of data find the servers with his ip address, then the server sends each blocs to the good process with the port number.


```c
// This code is for client, not for server and he don't listen the response ==> this is useless

#define MAX_SIZE 512

int                 MasterSocket;
struct sockaddr     serveradd;
char                buff[MAX_SIZE];
size_t              bufflen;

bzero(buff, MAXSIZE);
// Prepare and connect to socket
MasterSocket = socket(AF_INET, SOCK_STREAM, 0);  // AF_INET == use ipv4, SOCK_STREAM == can write and read, 0 == TCP
if (MasterSocket < 0)
  err_stop();
serveradd.sin_family = AF_INET;
serveradd.sin_port = htons(SERVER_PORT);

if (connect(MasterSocket, &serveradd, sizeof(serveradd)) < 0)
  err_connect();

//Prepare and send msg
buff = "SALUT !!\r\n" // I know this will not works
bufflen = strlen(buff);

if (send(MasterSocket, &buff, bufflen, MSG_OOB) != bufflen) // MSG_OOB == out of band data, don't know what it is
  err_sending();
```

## Client
### Irssi
[doc](https://irssi.org/documentation/)
Install on Linux
```bash
sudo apt install irssi
```
Install in 42
```bash
brew install irssi
```
#### Connect Irssi to our server
In irssi[^client]
```bash
/network add ft_irc
/server add -network ft_irc 127.0.0.1 6697
#start the server in shell with ./ft_irc 6697 password
/connect ft_irc
```



## Search
### Questions
? Does we need to implement :
- services

### Server
__Name maximum length 63 characters__[^RFC2812_1_1]

### Channels
> "A channel is a named group of one or more users which will all
> receive messages addressed to that channel.  A channel is
> characterized by its name, properties and current members."

__Name maximum length 50 char__ beginning with '&', '#', '+', or '!', without spaces ' ', a control G (ASCII 7) or a comma ',' 
Channel names are case insensitive.[^RFC2812_1_3]

Standard channels ('&', '#', '+') are implicitly created when first user joins it and die when the last user leaves it.[^RFC2811_3_1]
Safe channels ('!') are not implicitly created. The user must request the creation using JOIN with channel id replace by '!'. The name of this channel is the concatenation of 5 characters + x characters (user chooses). The user become "channel creator". Theses channels may continue to exist after the last user left.

Cf Creator to see special habilities of them 
Cf Operators to see special habilities of them

#### Channel Modes
For users : 
  -O, -o, -V

For channels : 
  -a, -i, -m, -n, -q, -p, -s, -r, -t, -k, -l, -b, -e, -I (see details in [RFC2811 4](https://www.rfcreader.com/#rfc2811_line281))

All these modes can be manipulated by "channel operators" by using MODE command

### User
Each user is distinguished from other by a unique nickname, __max length 9 characters__ (always real ?)[^RFC2812_1_2_1]

#### Operators
They are clients with specials habilities (it's looks like administrors)

Channel operator (chop or chanop) is the "owner" of that channel. The '@' is next to his nickname
He can[^RFC1459_1_3_1] :
- KICK (RFC1459 4.2.8)
- MODE (RFC1459 4.2.3)
- INVITE (RFC1459 4.2.7) (mode +i (RFC2811 2.4))
- TOPIC (RFC1459 4.2.4)
Not needed :
- SQUIT (RFC2812 3.1.8) because no server-server communication

Check __Messages__ section for more details

The granting of operator privilegs need some requirements[^RFC1459_8_12_2]

#### Creator
A user who create a channel with the character '!' as prefix is the "channel creator" (-O). Upon creation of the channel,
this user is also given channel operator status.[^RFC2811_2_4_2]

### Messages
- 3 mains parts :
  - the prefix (optional),
  - the command
  - the command params (up to 15)

They are separated by unless one space (ASCII 0x20).[^RFC1459_2_3]

- IRC messages are always lines of characters __(512 max)__ with a CR-LF (Carriage Return - Line Feed) __(\r\n)__ pair.[^RFC1459_2_3] 
Not all implementations use CR-LF[^RFC1459_8]
- see pseudo messages in [here](https://www.rfcreader.com/#rfc1459_line350)

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
[RFC1459 section 6](https://www.rfcreader.com/#rfc1459_line1993), we just have to copie paste all in a file

### Funny things
> "Because of IRC's scandanavian origin, the characters {}| are
> considered to be the lower case equivalents of the characters []\,
> respectively. This is a critical issue when determining the
> equivalence of two nicknames."[^RFC1459_2_2]

## Sources

[socket_prog_fr](http://tvaira.free.fr/bts-sn/reseaux/cours/cours-sockets.pdf)

[socket_prog](https://beej.us/guide/bgnet/html)

[poll()](https://www.ibm.com/docs/en/i/7.1?topic=designs-using-poll-instead-select)


[before_RFC](http://chi.cs.uchicago.edu/chirc/intro.html)

[RFC1459:Base](https://datatracker.ietf.org/doc/html/rfc1459)
[RFC2810:Architecture](https://datatracker.ietf.org/doc/html/rfc2810)
[RFC2811:ChanneManagement](https://datatracker.ietf.org/doc/html/rfc2811)
[RFC2812:ClientProtocole](https://datatracker.ietf.org/doc/html/rfc2812)
[RFC2813:ServerProtocole](https://datatracker.ietf.org/doc/html/rfc2813)
[RFC7194:TLS/SSL](https://www.rfcreader.com/#rfc7194)


[^1]: https://fr.wikipedia.org/wiki/Port_(logiciel)
[^client]: https://irssi.org/New-users/
[^RFC2812_1_1]: https://www.rfcreader.com/#rfc2812_line126
[^RFC2812_1_3]: https://www.rfcreader.com/#rfc2812_line176
[^RFC2811_3_1]: https://www.rfcreader.com/#rfc2811_line219
[^RFC2812_1_2_1]: https://www.rfcreader.com/#rfc2812_line138
[^RFC1459_1_3_1]: https://www.rfcreader.com/#rfc1459_line263
[^RFC1459_8_12_2]: https://www.rfcreader.com/#rfc1459_line2922
[^RFC2811_2_4_2]: https://www.rfcreader.com/#rfc2811_line199
[^RFC1459_2_3]: https://www.rfcreader.com/#rfc1459_line311
[^RFC1459_8]: https://www.rfcreader.com/#rfc1459_line2654
[^RFC1459_2_2]: https://www.rfcreader.com/#rfc1459_line294