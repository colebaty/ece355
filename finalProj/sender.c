#ifndef _GNU_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

/* 0xe10c in decimal */
#define PORT "56712"

/* 
    commmon variables, declared here because i'm lazy
    and don't want to worry about passing them around
 */
int sockfd;
struct addrinfo hints, *servinfo, *p;
int rv, numbytes, totalbytes = 0;

/* message to send to receiver - specified by instruction document */
char *msg_out = "Testing rdt1.0 protocol\0";

/**
 * @brief mkd_pkt, specified by instruction document. since we're just 
 * sending one char at a time, this function is not strictly needed, but
 * since the instructions specify it, here it is
 * 
 * @param data 
 * @return char 
 */
char make_pkt(char data) {
    return data;
};

/**
 * @brief udt_send function specified by instructions. essentially a wrapper
 * function for sendto() from sys/socket.h
 * 
 * @param packet 
 */
void udt_send(char packet) {
    if ((numbytes = sendto(sockfd, &packet, 1, 0, 
            p->ai_addr, p->ai_addrlen)) == -1) {
        perror("sender: sendto");
        exit(1);
    }

    totalbytes++;
};

/**
 * @brief rdt_send() function. calls udt_send() with specified
 * data.
 * 
 * @param data 
 */
void rdt_send(char data) {
    char packet = make_pkt(data);
    udt_send(packet);
};

int main(int argc, char **argv) {

    /*
        begin initilization code. this comes almost verbatim from the resource doucment 
        mentioned in the report. initializes data strcutres required for socket programming
        on Unix systems, as well as some error checking/handling
     */
    if (argc != 2) {
        fprintf(stderr, "usage: sender hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("sender: socket");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "sender: failed to bind\n");
        return 2;
    }
    /* end initiliaztion code */

    /* BEGIN RDT1.0 SEND LOOP */
    for (int i = 0; i < strlen(msg_out); i++) {
        printf("sender: sending \"%c\"\n", (char) msg_out[i]);
        rdt_send(msg_out[i]);
        sleep(1);
    }
    printf("\n");
    /* END RDT1.0 SEND LOOP */
    

    /* legacy code from resource document */
    freeaddrinfo(servinfo);

    // printf("sender: sent %d bytes to %s\n", totalbytes, argv[1]);

    /* send 0-length packet to terminate connection */
    sendto(sockfd, "", 0, 0, p->ai_addr, p->ai_addrlen);
    /* close socket */
    close(sockfd);

    return 0;

}

#endif // _GNU_SOURCE