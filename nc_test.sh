#!/bin/bash

# PORT="6697"
HOST="127.0.0.1"

CR=$'\r'
LF=$'\n'

nc $HOST 6697 <<EOF
CMD1 argument lol$CR$LF:prefix CMD2$CR$LF
EOF

exit 0