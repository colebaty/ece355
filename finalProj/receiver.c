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

/* 0xe10c in decimal */
#define PORT "56712"

/* 100 is arbitrary but useful */
#define MAXBUFLENGTH 100

/* 
    commmon variables, declared here because i'm lazy
    and don't want to worry about passing them around
 */
int sockfd, numbytes, rv;
struct addrinfo hints, *servinfo, *p;
char buf[MAXBUFLENGTH], data;
struct sockaddr_storage their_addr;
socklen_t addr_len;
char s[INET6_ADDRSTRLEN];

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/**
 * @brief rdt1.0 extract function; assigns the single char
 * sent through the socket to the data element
 * 
 * @param packet 
 * @param data 
 */
void extract(char *packet, char *data) {
    *data = *packet;
};

/**
 * @brief 'delivers' the data to the next layer; i.e. prints to screen
 * 
 * @param data 
 */
void deliver_data(char *data) {
    printf("receiver: packet contains \"%c\"\n", *data);
}

/**
 * @brief rdt_recv function specified by instructions. bascially a wrapper
 * for recvfrom() from sys/socket.h. returns '0' to indicate the connection has been closed,
 * so the sender will need to explicitly send a packet of 0 length to terminate
 * the connection
 * 
 * @return int 
 */
int rdt_recv() {
    numbytes = recvfrom(sockfd, buf, MAXBUFLENGTH-1, 0, 
                (struct sockaddr*) &their_addr, &addr_len);

    /* error handling per resource document */
    if (numbytes == -1) {
        perror("recv");
        exit(1);
    }

    /*  legacy code from resource document - uncomment to see ip addr of sender */
    // printf("receiver: got packet from %s\n", 
    //     inet_ntop(their_addr.ss_family, 
    //         get_in_addr((struct sockaddr *)&their_addr), 
    //         s, sizeof s));

    /* uncomment for length of reeived packet */
    // printf("receiver: packet is %d bytes long\n", numbytes);
    
    /* null terminator for c-string compliance. will not work in printf() without null terminator*/
    buf[numbytes] = '\0';

    /* CALL TO RDT1.0 EXTRACT FUNCTION */
    extract(&buf[0], &data);

    /* CALL TO RDT1.0 DELIVER DATA FUNCTION */
    deliver_data(&data);

    return numbytes;
};

int main(int argc, char **argv)
{
    /*
        begin initilization code. this comes almost verbatim from the resource doucment 
        mentioned in the report. initializes data strcutres required for socket programming
        on Unix systems, as well as some error checking/handling
     */
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
            perror("receiver: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
            close(sockfd);
            perror("receiver: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "receiver: failed to connect\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("receiver: waiting to recv from...\n");

    addr_len = sizeof their_addr;
    /* end initialization code */

    /* greek tragedy - all the action happens offstage */
    while (rdt_recv() > 0);/* <--- CALL TO RDT1.0 RDT_RECV FUNCTION */

    /* closes socket */
    close(sockfd);

    return 0;
}

# endif // _GNU_SOURCE