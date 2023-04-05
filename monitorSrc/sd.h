#pragma once

/// @brief Initializes the Multicast socket used for Service Discovery
void sd_initMcSock(void);

/// @brief Retreives monitor's local IP address, and starts the advertising thread which multicasts the IP address 
void sd_startAdvertising(void);
