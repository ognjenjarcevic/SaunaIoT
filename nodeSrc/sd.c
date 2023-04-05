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

/// @brief Port on the SD node that used to receive multicast messages from the central
/// @note Needs to be the same as Monitor's MULTICAST_PORT
#define MULTICAST_PORT 12346

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

    /// @brief Port for exchanging messages with SD central
    int portCen;

    /// @brief IP address of the SD Central
    char ipCen[32];

    /// @brief Message received from SD central on the Multicast channel
    char messageMc[32];

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
    context.addrMc.sin_port = htons(MULTICAST_PORT);

    //context.addrMc.sin_addr.s_addr = inet_addr(SD_MULTICAST_GROUP);
    if (inet_aton(MULTICAST_GROUP , &context.addrMc.sin_addr) == 0) 
	{
		fprintf(stderr, "inet_aton() failed\n");
		return;
	}

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
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
    printf("Finding SD central...\n");
    while(1)
    {
        // send message
        if (recvfrom(context.sockMc, context.messageMc, sizeof(context.messageMc), 0, (struct sockaddr*)&context.addrMc, &context.addrLen) < 0)
        {
            printf("Error receiving SD message..\n");
            //exit(EXIT_FAILURE);
        }
        else
        {
            printf("SD message received: %s\n", context.messageMc);
            // Parse the message into the ipAddress, and port to be targeted

            // TODO: Basic check can be performed if the received message is an IP address
            // Check successful
            break;
        }
        sleep(1);
    }
}

void sd_initCenSock(void)
{
    int sockfd;

    sscanf(context.messageMc, "%s %d", context.ipCen, &context.portCen);

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("Socket init: creation failed\n");
        return;
    }

    // Set up the address
    context.addrCen.sin_family = AF_INET;
    context.addrCen.sin_port = htons(context.portCen);
    context.addrCen.sin_addr.s_addr = inet_addr(context.ipCen);

    // if (bind(sockfd, (struct sockaddr *)&context.addrCen, context.addrLen) != 0) {
    //     printf("Socket init: bind failed\n");
    //     return;
    // }

    context.sockCen = sockfd;
}

static void *sd_sendStatusUpdate(void* parm)
{
    while(1)
    {
        char updateMessage[] = "Still alive";
        if (sendto(context.sockCen, updateMessage, strlen(updateMessage)+1, 0, (struct sockaddr*)&context.addrCen, context.addrLen) < 0)
        {
            printf("sendto failed\n");
            //exit(EXIT_FAILURE);
        }
        printf("\tSent alive..\n\n");
        sleep(2);
    }
    return NULL;
}

void sd_startSendingUpdates(void)
{
    printf("SD sending alive messages start..\n");

    int status = pthread_create(&context.updateThread, NULL, sd_sendStatusUpdate, NULL);
    if(status < 0)
        printf("Alive pthread_create failed");
    // else
    //     printf("pthread_create success\n");

    status = pthread_detach(context.updateThread);
    if(status < 0)
        printf("Alive pthread_detach failed");
    // else
    //     printf("pthread_detach success\n");
}