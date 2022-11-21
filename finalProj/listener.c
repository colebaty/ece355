#ifndef _GNU_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT "56712"

#define MAXBUFLENGTH 100

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char **argv)
{
    int sockfd, numbytes, rv;
    struct addrinfo hints, *servinfo, *p;
    char buf[MAXBUFLENGTH];
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
            close(sockfd);
            perror("listener: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to connect\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("listener; waiting to recv from...\n");

    addr_len = sizeof their_addr;

    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLENGTH-1, 0,
        (struct sockaddr*) &their_addr, &addr_len)) == -1) {
        perror("recv");
        exit(1);
    }

    printf("listener: got packet from %s\n", 
        inet_ntop(their_addr.ss_family, 
            get_in_addr((struct sockaddr *)&their_addr), 
            s, sizeof s));

    printf("listener: packet is %d bytes long\n", numbytes);
    buf[numbytes] = '\0';

    printf("listner: packet contains \"%s\"\n", buf);

    close(sockfd);

    return 0;
}

# endif // _GNU_SOURCE