# Lexer / Parser for IRC

Our lexer can get more than 1 command, we must make something like gnl
Use std::queue 

- keywords :
    - cf list commands
- separators :
    - :     // use to start prefix
    - ' '   // space 0x20
- operators :
    - CR LF // "\r\n" says the end of message


It's looks like if first param is ok the others bad params are ignored


