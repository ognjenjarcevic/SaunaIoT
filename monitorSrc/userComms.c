#include "userComms.h"
#include <string.h>
#include <stdio.h>

static int tempVal = 0;
static int humVal = 0;


static char reply[50];

char* userComms_replMess(void)
{
    return reply;
}

int userComms_recv(char* mess)
{
    // The message will be in the following format
    // user <"temp" or "hum"> <number>

    char user[6];
    char function[6];
    int value = 0;
    sscanf(mess, "%s %s %d", user, function, &value);

    if (strcmp(user, "user") != 0)
    {
        printf("ERROR: The received message is not from the user!\n");
        return -1;
    }
    
    if (strcmp(function, "temp") == 0)
    {
        tempVal = value;
        snprintf(reply, 50, "Temp set to %d", tempVal);
        printf("%s\n", reply);

        return 0;
    }
    else if (strcmp(function, "hum") == 0)
    {
        humVal = value;
        snprintf(reply, 50, "Hum set to %d", humVal);
        printf("%s\n", reply);

        return 0;
    }
    else
    {
        printf("ERROR: The received message does not set either the hum or temp!\n");
        return -1;
    }
    // Set the value
}