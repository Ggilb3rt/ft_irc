#!/bin/bash

# PORT="6697"
HOST="127.0.0.1"
LOCALHOST="localhost"

CR=$'\r'
LF=$'\n'

#use -c to send '\r\n' instead of '\n'
nc -c $LOCALHOST 6697 <<EOF
CMD1 argument lol$CR$LF:prefix CMD2$CR$LF
EOF


#another way
#nc -c $HOST 6697 < msg.txt


echo "end"

exit 0