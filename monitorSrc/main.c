#include "sd.h"

int main()
{   

    sd_initMcSock();

    sd_startAdvertising();




    // Dont end the program
    while(1)
    {

    }
    return 0;
}
