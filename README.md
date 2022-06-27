# ft_irc
My own irc server

## Search
### Operators
They are clients with specials habilities (like administrors)

Channel operator (chop or chanop) is the "owner" of that channel. The '@' is next to his nickname
He can :
- KICK
- MODE
- INVITE
- TOPIC
(cf RFC1459 1.3.1)

### Messages
3 mains parts : the prefix (optional), the command and the command params (up to 15). They are separated by unless one space (ASCII 0x20). (cf RFC1459 2.3)

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