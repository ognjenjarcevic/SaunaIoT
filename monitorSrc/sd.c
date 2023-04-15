#include "sd.h"
#include "sdMess.h"
#include "sdTracking.h"

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

/// @brief Port on the multicast group that the messages are being sent to
/// @note Needs to be the same as Node's MULTICAST_PORT
#define MULTICAST_PORT 12346

/// @brief Port on the SD central that used to receive unicast messages from the nodes
#define RECEIVER_PORT 12345
#define RECEIVER_PORT_STR "12345"

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

    /// @brief Message that is sent as an advertising packet (contains IP addr + recv sock)
    char advertMessage[64];

    pthread_t updateThread;
    pthread_t receivingThread;

    char receiver[32];

};
static struct Context context = {
    .addrLen = sizeof(struct sockaddr_in),
};

void sd_initIpAddress(void)
{
    // Getting IP address out of the local DNS Server shenanigans
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

    // Creating the message used for advertising
    snprintf(context.advertMessage, sizeof(context.advertMessage), "%s %s", context.ipCen, RECEIVER_PORT_STR);
}

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

void sd_initUnicastSock(void)
{
    int sockfd;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        printf("Socket init: creation failed\n");
        return;
    }

    // Set up the address
    memset(&context.addrCen, 0, sizeof(context.addrCen));
    context.addrCen.sin_family = AF_INET;
    context.addrCen.sin_port = htons(RECEIVER_PORT);
    context.addrCen.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&context.addrCen, context.addrLen) != 0) {
        printf("Socket init: bind failed\n");
        return;
    }

    printf("Unicast socket initialized, %d\n", sockfd);
    context.sockSens = sockfd;
}

/// @brief To be used in a separate thread, constantly multicasts the SD adverts
static void* sd_multicastAdvert(void* parm)
{
    while(1)
    {
        if (sendto(context.sockMc, context.advertMessage, strlen(context.advertMessage)+1, 0, (struct sockaddr*)&context.addrMc, context.addrLen) < 0)
        {
            printf("sendto failed\n");
            return NULL;
        }
        //printf("SD multicast adveritising\n");
        //printf("sent %s\n", context.advertMessage);
        sleep(SD_MON_AD_PERIOD);
    }
    return NULL;
}

void sd_startAdvertising(void)
{   
    // Creating the thread used for constant advertising
    printf("SD advertising start..\n");
    int status = pthread_create(&context.updateThread, NULL, sd_multicastAdvert, NULL);
    if(status < 0)
        printf("Advertising pthread_create failed\n");

    status = pthread_detach(context.updateThread);
    if(status < 0)
        printf("Advertising pthread_detach failed\n");
}


static void* sd_receiveMessage(void* parm)
{
    while(1)
    {
        if (recvfrom(context.sockSens, context.receiver, sizeof(context.receiver), 0, (struct sockaddr*)&context.addrCen, &context.addrLen) < 0)
        {
            printf("Error receiving SD message..\n");
        }
        //printf("received: %s\n", context.receiver);

        SDMessage s = -1;
        // Decode the received message
        if (strncmp(context.receiver, SD_SEN_ALIVE_STR, sizeof(context.receiver)) == 0)
        {
            s = SD_SEN_ALIVE;
        }
        if (strncmp(context.receiver, SD_SEN_BYEBYE_STR, sizeof(context.receiver))== 0)
        {
            s = SD_SEN_BYEBYE;
        }
        if (strncmp(context.receiver, SD_ACT_ALIVE_STR, sizeof(context.receiver))== 0)
        {
            s = SD_ACT_ALIVE;
        }
        if (strncmp(context.receiver, SD_ACT_BYEBYE_STR, sizeof(context.receiver)) == 0)
        {
            s = SD_ACT_BYEBYE;
        }

        sdTracking_update(s);
    }

    return NULL;
}

void sd_startReceiving(void)
{
    printf("SD Receiving alive messages start..\n");

    int status = pthread_create(&context.receivingThread, NULL, sd_receiveMessage, NULL);
    if(status < 0)
        printf("SD Receiving pthread_create failed");
    // else
    //     printf("pthread_create success\n");

    status = pthread_detach(context.receivingThread);
    if(status < 0)
        printf("SD Receiving pthread_detach failed");
    // else
    //     printf("pthread_detach success\n");
}