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

#define PORT "56712"

int sockfd;
struct addrinfo hints, *servinfo, *p;
int rv, numbytes, totalbytes = 0;

char *msg_out = "Testing rdt1.0 protocol\0";

char make_pkt(char data) {
    return data;
};

void udt_send(char packet) {
    if ((numbytes = sendto(sockfd, &packet, 1, 0, 
            p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }

    totalbytes++;
};

void rdt_send(char data) {
    char packet = make_pkt(data);
    udt_send(packet);
};

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "usage: talker hostname\n");
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
            perror("talker: socket");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind\n");
        return 2;
    }

    printf("sending...");
    for (int i = 0; i < strlen(msg_out); i++) {
        rdt_send(msg_out[i]);
        printf("%c ", (char) msg_out[i]);
        sleep(1);
    }
    printf("\n");
    

    freeaddrinfo(servinfo);

    printf("talker: sent %d bytes to %s\n", totalbytes, argv[1]);
    /* send 0-length packet to terminate connection */
    sendto(sockfd, "", 0, 0, p->ai_addr, p->ai_addrlen);
    close(sockfd);

    return 0;

}

#endif // _GNU_SOURCE