#include "sd.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    //argv[1] is either "sensor" or "actuator"
    if(argv[1][0] != 's' && argv[1][0] != 'a')
    {
        printf("invalid role argument..\n");
        return -1;
    }
    sd_initRole(argv[1]);

    sd_initMcSock();

    sd_findCentral();
    
    sd_initCenSock();

    sd_startSendingUpdates();

    //logika aktuatora ili logika senzora

    // Dont end the program
    // while(1)
    // {

    // }
    printf("\t***********************************************\n");
    printf("\tPress any key to send a byebye message and quit\n");
    printf("\t***********************************************\n");
    getchar();
    
    sd_sendByeBye();
    return 0;
}
