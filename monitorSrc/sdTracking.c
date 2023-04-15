#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "sdTracking.h"

#define SD_LIFETIME 7

typedef enum DevState
{
    /// @brief Initial state
    DEV_STATE_NOT_FOUND,

    /// @brief Alive after receiving the alive message
    DEV_STATE_ALIVE,

    /// @brief Happens upon "byebye" message while alive (node sends "byebye message")
    DEV_STATE_BYEBYE, 

    /// @brief Dies when alive message not received recently (unexpected node shutdown)
    DEV_STATE_DEAD,
} DevState;


struct Context
{
    DevState senState;

    DevState actState;

    int senLifetime;

    int actLifetime;

    pthread_t tickThread;

};

static struct Context context =
{
    .senState = DEV_STATE_NOT_FOUND,
    .actState = DEV_STATE_NOT_FOUND,
    
    .senLifetime = 0,
    .actLifetime = 0,
};

void* sdTracking_tick(void* parm)
{
    while(1)
    {
        if (context.actState == DEV_STATE_ALIVE)
        {
            if (context.actLifetime > 0)
            {
                context.actLifetime--;
                printf("\t\tAct life: %d\n", context.actLifetime);
            }

            if (context.actLifetime == 0)
            {
                context.actState = DEV_STATE_DEAD;
                printf("\tActuator node died!\n");
            }
        }

        if (context.senState == DEV_STATE_ALIVE)
            {
            if (context.senLifetime > 0)
            {
                context.senLifetime--;
                printf("\t\tSen life: %d\n", context.senLifetime);
            }

            if (context.senLifetime == 0)
            {
                context.senState = DEV_STATE_DEAD;
                printf("Sensor node died!\n");
            }
        }

    sleep(1);
    }

    return NULL;
}

void sdTracking_start(void)
{   
    // Creating the thread used for constant advertising
    printf("SD tracking start..\n");
    int status = pthread_create(&context.tickThread, NULL, sdTracking_tick, NULL);
    if(status < 0)
        printf("Tracking pthread_create failed\n");

    status = pthread_detach(context.tickThread);
    if(status < 0)
        printf("Tracking pthread_detach failed\n");
}

void sdTracking_update(SDMessage mess)
{
    if (mess == SD_ACT_ALIVE)
    {
        if (context.actState == DEV_STATE_NOT_FOUND)
        {
            printf("\tActuator node found!\n");
        }
        printf("\t\tActuator node alive.\n");
        context.actState = DEV_STATE_ALIVE;
        context.actLifetime = SD_LIFETIME;
    }

    if (mess == SD_SEN_ALIVE)
    {
        if (context.senState == DEV_STATE_NOT_FOUND)
        {
            printf("\tSensor node found!\n");
        }
        printf("\t\tSensor node alive.\n");
        context.senState = DEV_STATE_ALIVE;
        context.senLifetime = SD_LIFETIME;
    }

    if (mess == SD_ACT_BYEBYE)
    {
        printf("\tActuator node byebye!\n");
        context.actState = DEV_STATE_BYEBYE;
        context.actLifetime = 0;
    }

    if (mess == SD_SEN_BYEBYE)
    {
        printf("\tSensor node byebye!\n");
        context.senState = DEV_STATE_BYEBYE;
        context.senLifetime = 0;
    }
}
