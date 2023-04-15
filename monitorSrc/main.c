#include "sd.h"
#include "sdTracking.h"

int main()
{   
    sd_initIpAddress();

    sd_initMcSock();

    sd_initUnicastSock();
    
    sd_startReceiving();

    sd_startAdvertising();

    sdTracking_start();
    



    // Dont end the program
    while(1)
    {

    }
    return 0;
}
