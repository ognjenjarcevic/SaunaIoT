#include "sd.h"

int main()
{   
    //sd_setRole("aktuator");
    sd_initMcSock();

    sd_findCentral();
    
    sd_initCenSock();

    sd_startSendingUpdates();

    //logika aktuatora ili logika senzora

    // Dont end the program
    while(1)
    {

    }
    return 0;
}
