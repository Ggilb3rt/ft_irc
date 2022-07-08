// #include <iostream>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <sys/resource.h>


// int main() {
//     rlimit rlp;

//     getrlimit(RLIMIT_NOFILE, &rlp);

//     std::cout << rlp.rlim_cur << " " << rlp.rlim_max << std::endl;

//     int useless_fd[rlp.rlim_cur];

//     for (int i = 0; i < rlp.rlim_cur; i++) { // if rlim_cur + 3 ==> segfault (I think it caused by the acces of fd 1 from amother process)
//         useless_fd[i] = open("client.c", O_APPEND);
//         if (useless_fd[i] == -1)
//             std::cout << "(" << errno << ")" ;
//         std::cout << useless_fd[i] << ";";
//     }
//     std::cout << std::endl;

//     // while (1) {}

//     for (int i = 0; i < rlp.rlim_cur; i++) {
//         close(useless_fd[i]);
//     }
// }







/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <arpa/inet.h>

#define PORT "6697" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    printf("Bonjouerpouet\r\nlol internet\n");
    int _master_sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    int co_ret;

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    std::string ret;
    for(p = servinfo; p != NULL; p = p->ai_next) {
        for (int i = 0; i < 1020; i++) {
            ret = std::to_string(i) + " Yo\n";
            if ((_master_sockfd = socket(p->ai_family, p->ai_socktype,
                    p->ai_protocol)) == -1) {
                perror("client: socket");
                continue;
            }
            std::cout << "master socket " << _master_sockfd << std::endl;
            // fcntl(_master_sockfd, F_SETFL, O_NONBLOCK);	// disable the capacity to block from accept() recv() etc // need to check errors
            co_ret = connect(_master_sockfd, p->ai_addr, p->ai_addrlen);
            if (co_ret == -1) {
                close(_master_sockfd);
                perror("client: connect");
                continue;
            }
            std::cout << "connect " << co_ret << std::endl;
            send(_master_sockfd, ret.c_str(), ret.length(), 0);
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    // send(_master_sockfd, "Yo !\n", 5, 0);
    // sleep(1);
    // printf("awake\n");
    // send(_master_sockfd, "Yo2 !\n", 6, 0);
    // send(_master_sockfd, "|Yo3 !\rP\n|", 10, 0);
    // send(_master_sockfd, "|Yo4 !\rP\n|", 10, 0);
    // inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
    //         s, sizeof s);
    // printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    if ((numbytes = recv(_master_sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("client: received '%s'\n",buf);

    while (1){}
    close(_master_sockfd);

    return 0;
}


