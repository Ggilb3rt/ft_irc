# ft_irc
My own irc server

## Search
### Questions
Does we need to implement :
- channel mode (RFC1459 4.2.3.1) (yes cf operators)
- user mode (RFC1459 4.2.3.2) (yes cf operators)

### Operators
They are clients with specials habilities (it's looks like administrors)

Channel operator (chop or chanop) is the "owner" of that channel. The '@' is next to his nickname
He can :
- KICK (4.2.8)
- MODE (4.2.3)
- INVITE (4.2.7)
- TOPIC (4.2.4)
(cf RFC1459 1.3.1)

### Messages
- 3 mains parts : the prefix (optional), the command and the command params (up to 15). They are separated by unless one space (ASCII 0x20). (cf RFC1459 2.3)
- IRC messages are always lines of characters (512 max) with a CR-LF (Carriage Return - Line Feed) pair. (cf RFC1459 2.3) Not all implementations use CR-LF (cf RFC1459 8.)
- see pseudo messages in RFC1459 2.3.1

Needed commands (cf RFC1459 4.x) :
- Connection (4.1)
  - (PASS ??)
  - NICK
  - USER
  - QUIT
- Channel (4.2)
  - JOIN
- Sending messages (4.4)
  - PRIVMSG
- plus all operators specials habilities

### Error replies
cf RFC1459 section 6, we just have to copie paste all in a file

### Funny things
> "Because of IRC's scandanavian origin, the characters {}| are
> considered to be the lower case equivalents of the characters []\,
> respectively. This is a critical issue when determining the
> equivalence of two nicknames." (cf RFC1459 2.2)

## Sources
[RFC1459:Base]https://datatracker.ietf.org/doc/html/rfc1459
[RFC2810:Architecture]https://datatracker.ietf.org/doc/html/rfc2810
[RFC2811:ChanneManagement]https://datatracker.ietf.org/doc/html/rfc2811
[RFC2812:ClientProtocole]https://datatracker.ietf.org/doc/html/rfc2812
[RFC2813:ServerProtocole]https://datatracker.ietf.org/doc/html/rfc2813
[RFC7194:TLS/SSL]https://www.rfcreader.com/#rfc7194