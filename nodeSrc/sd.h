#pragma once

#define SD_NODE_ALIVE_PERIOD 2

void sd_initRole(char* argRole);

void sd_initMcSock(void);

void sd_findCentral(void);

void sd_initCenSock(void);

void sd_startSendingUpdates(void);

void sd_sendByeBye(void);
