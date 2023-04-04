#include "sd.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

void find_broker(int* s)
{

    printf("Node program started\n");
    int sockfd;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    // create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("socket creation failed\n");
    }

    // set up multicast address to send to
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    // enable multicast on socket
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        printf("setsockopt failed\n");
    }

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("bind");
        //exit(EXIT_FAILURE);
    }

    printf("Node setup finished, receiving messages\n");

    char message[1024];
    while(1)
    {
        // send message
        if (recvfrom(sockfd, message, sizeof(message), 0, (struct sockaddr*)&addr, &addrlen) < 0)
        {
            printf("recv failed\n");
            //exit(EXIT_FAILURE);
        }
        printf("Received %d bytes: %s\n", strlen(message)+1, message);
    }

    *s = sockfd;

}