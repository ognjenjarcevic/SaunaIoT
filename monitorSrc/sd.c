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
#include <pthread.h>

#define MULTICAST_GROUP "239.1.1.1"
#define PORT 12346

struct Context
{
    /// @brief 
    socklen_t addrLen;

    /// @brief Socket for sending messages to the Multicast channel
    int sockMc;

    /// @brief Address for sending messages to the Multicast channel
    struct sockaddr_in addrMc;

    /// @brief Socket for exchanging messages with the Sensor node
    int sockSens;

    /// @brief Address for exchanging messages with the SD Central
    struct sockaddr_in addrCen;

    /// @brief IP address of the SD Central
    char ipCen[32];

    pthread_t updateThread;

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
    context.addrMc.sin_port = htons(PORT);
    context.addrMc.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);

    // Multicast settings for the address
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        printf("setsockopt failed\n");
        return;
    }

    // Bind is not neccesary, as monitor doesn't need to receive the messages from the MC channel

    
    printf("Multicast socket initialized\n");
    context.sockMc = sockfd;
}

/// @brief To be used in a separate thread, constantly multicasts the SD adverts
static void *sd_multicastAdvert(void* parm)
{
    while(1)
    {
        if (sendto(context.sockMc, context.ipCen, strlen(context.ipCen)+1, 0, (struct sockaddr*)&context.addrMc, context.addrLen) < 0)
        {
            printf("sendto failed\n");
            //exit(EXIT_FAILURE);
        }
        printf("sent %s\n", context.ipCen);
        sleep(1);
    }
}

void sd_startAdvertising(void)
{   
    struct hostent* IPstructure;
    struct in_addr** addr_list;
    char hostname[32];
    int i;

    char localPostfix[] = ".local";

    gethostname(hostname, 32);
    strncat(hostname, localPostfix, sizeof(hostname));

    IPstructure = gethostbyname(hostname);
    addr_list = (struct in_addr **) IPstructure->h_addr_list;
    for (i = 0; addr_list[i] != NULL; i++)
    {
        strcpy(context.ipCen , inet_ntoa(*addr_list[i]));
    }

    printf("%s resolved to : %s\n", hostname, context.ipCen);
    printf("SD advertising start..\n");

    int status = pthread_create(&context.updateThread, NULL, sd_multicastAdvert, NULL);

    if(status < 0)
        printf("pthread_create failed");
    else
        printf("pthread_create success\n");

    status = pthread_detach(context.updateThread);

    if(status < 0)
        printf("pthread_detach failed");
    else
        printf("pthread_detach success\n");
    
}