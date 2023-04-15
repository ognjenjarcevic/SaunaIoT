#pragma once

#define SD_MON_AD_PERIOD 3


/// @brief Initializes the Multicast socket used for Service Discovery
void sd_initMcSock(void);

void sd_initIpAddress(void);

/// @brief Retreives monitor's local IP address, and starts the advertising thread which multicasts the IP address 
void sd_startAdvertising(void);

void sd_initUnicastSock(void);

void sd_startReceiving(void);
