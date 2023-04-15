#pragma once

typedef enum SDMessage
{
    SD_ACT_ALIVE,
    SD_ACT_BYEBYE,

    SD_SEN_ALIVE,
    SD_SEN_BYEBYE,
    
} SDMessage;

void sdTracking_update(SDMessage mess);

void sdTracking_start(void);