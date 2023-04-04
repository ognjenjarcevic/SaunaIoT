#include "sd.h"


#define MULTICAST_GROUP "239.0.0.1"
#define PORT 12345

int main()
{
    int sockfd;
    find_broker(&sockfd);
 

    close(sockfd);
    return 0;
}
