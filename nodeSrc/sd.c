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

#define SD_MULTICAST_GROUP "239.1.1.1"
#define SD_NODE_MC_PORT 12346

struct Context
{
    /// @brief 
    socklen_t addrLen;

    /// @brief Socket for receiving messages from the Multicast channel
    int sockMc;

    /// @brief Address for receiving messages from the Multicast channel
    struct sockaddr_in addrMc;

    /// @brief Socket for exchanging messages with the SD Central
    int sockCen;

    /// @brief Address for exchanging messages with the SD Central
    struct sockaddr_in addrCen;

    /// @brief IP address of the SD Central
    char ipCen[1024];
};
static struct Context context = {
    .addrLen = sizeof(struct sockaddr_in),
};

void sd_initMcSock(void)
{
    int sockfd;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        printf("Socket init: creation failed\n");
        return;
    }

    // Set up the address
    memset(&context.addrMc, 0, sizeof(context.addrMc));
    context.addrMc.sin_family = AF_INET;
    context.addrMc.sin_port = htons(SD_NODE_MC_PORT);

    //context.addrMc.sin_addr.s_addr = inet_addr(SD_MULTICAST_GROUP);
    if (inet_aton(SD_MULTICAST_GROUP , &context.addrMc.sin_addr) == 0) 
	{
		fprintf(stderr, "inet_aton() failed\n");
		return;
	}

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(SD_MULTICAST_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        printf("setsockopt failed\n");
        return;
    }

    if (bind(sockfd, (struct sockaddr *)&context.addrMc, context.addrLen) < 0) {
        printf("Socket init: bind failed\n");
        return;
    }
    printf("Sock init: Multicast inialized\n");
    context.sockMc = sockfd;
}

void sd_findCentral(void)
{   
    printf("finding central...\n");
    while(1)
    {
        // send message
        if (recvfrom(context.sockMc, context.ipCen, sizeof(context.ipCen), 0, (struct sockaddr*)&context.addrMc, &context.addrLen) < 0)
        {
            printf("Finding central: failed\n");
            //exit(EXIT_FAILURE);
        }
        else
        {
            printf("Central found: %s\n", context.ipCen);
        }
        sleep(1);
    }
}

void sd_initCenSock(void)
{
    int sockfd;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("Socket init: creation failed\n");
        return;
    }

    // Set up the address
    context.addrCen.sin_family = AF_INET;
    context.addrCen.sin_port = htons(SD_NODE_MC_PORT);
    context.addrCen.sin_addr.s_addr = inet_addr(context.ipCen);

    if (bind(sockfd, (struct sockaddr *)&context.addrCen, context.addrLen) != 0) {
        printf("Socket init: bind failed\n");
        return;
    }

    context.sockCen = sockfd;
}