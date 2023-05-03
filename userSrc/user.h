#pragma once

void menu();

void user_initMcSock(void);

void user_findCentral(void);

void user_initCenSock(void);

void send_temperature(int temp);

void send_humidity(int hum);

void recv_reply(void);

