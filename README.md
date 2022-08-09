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
Quick connection
```bash
/connect localhost <port> <password> <username>
```

```bash
/network add ft_irc
/server add -network ft_irc 127.0.0.1 6697 pass
/network add -nick newNick ft_irc #change default nick
/network add -autosendcmd '^JOIN lol' ft_irc # https://irssi.org/documentation/help/network/
#start the server in shell with ./ft_irc 6697 password
/connect ft_irc
```

## Terminal helpers
### netcat
nc ip port
-C is used to send "\r\n" instead of "\n"
-c is equivalent for mac

```bash
nc -C 127.0.0.1 6697
```
### Background / Foreground
Press CTRL+Z which will suspend the current foreground job.
You can find all background jobs with ```jobs``` then bring a certain job to the foreground with ```fg``` or kill it with ```kill```
```bash
nc -C 127.0.0.1 6697
#[CRTL+Z]
jobs # will print all jobs
fg %1 # will bring to the foreground the job 1 (prints before)
kill %3 # will kill job 3
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
  - (end with "\r\n")

They are separated by unless one space (ASCII 0x20).[^RFC1459_2_3]

- IRC messages are always lines of characters __(512 max)__ with a CR-LF (Carriage Return - Line Feed) __(\r\n)__ pair.[^RFC1459_2_3] 
Not all implementations use CR-LF[^RFC1459_8]
- see pseudo messages in [here](https://www.rfcreader.com/#rfc1459_line350)

Needed commands (cf RFC1459 4.x) :
__We have to debate about what is needed and what is not following the subject.__
- Connection (4.1)
  - [ ] PASS
  - [ ] NICK
  - [ ] USER
  - SERVER (not needed, only server-server)
  - OPER
  - [x] QUIT
  - SQUIT (not needed, only server-server or alias of QUIT)
- Channel (4.2)
  - [x] JOIN
  - [x] PART (need to sendMessageToAll)
  - [ ] MODE
  - [x] TOPIC
  - [x] NAMES
  - [x] LIST
  - [x] INVITE
  - [x] KICK
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
  - [ ] PRIVMSG (mode +n, +m, +v (RFC2811 2.4)) (! irssi use command "/MSG user message" syntax)
  - [ ] NOTICE
- User (4.5)
  - WHO
  - WHOIS
  - WHOWAS
- Miscellaneous messages (4.6)
  - KILL
  - PING
  - [x] PONG (return with arguments of recieved PING)
  - ERROR
- OPTIONALS
  - lol nope
- all operators specials habilities are needed

### Error replies
[RFC1459 section 6](https://www.rfcreader.com/#rfc1459_line1993), we just have to copie paste all in a file

### Funny things
> "Because of IRC's scandanavian origin, the characters {}| are
> considered to be the lower case equivalents of the characters []\,
> respectively. This is a critical issue when determining the
> equivalence of two nicknames."[^RFC1459_2_2]


## How BNF (Backus-Naur Form) works
It's a metalanguage to define the syntaxe of a language.[^BNF_wiki]
Exemple with if statement in c :
> <if_struct> ::= if "(" <condition> ")" "{" <instructions> "}"

BNF define in irc protocol :[^RFC1459_2_3]
> <message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
> <prefix>   ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
> <command>  ::= <letter> { <letter> } | <number> <number> <number>
> <SPACE>    ::= ' ' { ' ' }
> <params>   ::= <SPACE> [ ':' <trailing> | <middle> <params> ]
> <middle>   ::= <Any *non-empty* sequence of octets not including SPACE
               or NUL or CR or LF, the first of which may not be ':'>
> <trailing> ::= <Any, possibly *empty*, sequence of octets not including
                 NUL or CR or LF>
> <crlf>     ::= CR LF


Some help with symbols :
- ::=       ==> *left* is define by *right*
- < >       ==> nonterminal (variable)
- '' or ""  ==> something inside is litteral char
- |         ==> or
- ( )       ==> isolate what's inside
- [ ]       ==> element is optional
- { }       ==> element can be repeat


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
[^BNF_wiki]: https://fr.wikipedia.org/wiki/Forme_de_Backus-Naur