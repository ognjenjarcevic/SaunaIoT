#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MULTICAST_GROUP "239.0.0.1"
#define PORT 12345

int main()
{
    printf("start of monitoring\n");
    
    struct hostent* serverIPstructure;
    struct in_addr** addr_list;
    char IPaddress[1024];

    char hostname[1024];
    gethostname(hostname, 1024);
    char localPostfix[] = ".local";
    strncat(hostname, localPostfix, sizeof(hostname));

    printf("hostname: %s\n", hostname);

    serverIPstructure = gethostbyname(hostname);
    int i;
    addr_list = (struct in_addr **) serverIPstructure->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++) 
	{
		//Return the first one;
		strcpy(IPaddress , inet_ntoa(*addr_list[i]));
	}
    printf("%s resolved to : %s\n", hostname, IPaddress);


    int sockfd;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    // create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("socket creation failed\n");
        //exit(EXIT_FAILURE);
    }

    // set up multicast address to send to
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
    addr.sin_port = htons(PORT);

    // Enable multicast on socket
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        printf("setsockopt failed\n");
        //exit(EXIT_FAILURE);
    }

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("bind");
        //exit(EXIT_FAILURE);
    }

    printf("Monitoring setup finished, sending messages");

    while(1)
    {
        // send message
        if (sendto(sockfd, IPaddress, strlen(IPaddress)+1, 0, (struct sockaddr*)&addr, addrlen) < 0)
        {
            printf("sendto failed\n");
            //exit(EXIT_FAILURE);
        }

        printf("sent %s\n", IPaddress);
        sleep(1);
    }
    

    close(sockfd);
    return 0;
}
