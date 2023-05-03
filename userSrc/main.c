#include <stdio.h>
#include <stdlib.h>
#include "user.h"

int main(int argc, char** argv) {

    user_initMcSock();

    user_findCentral();
    
    user_initCenSock();

    int menu_choice;
    int input;

    do {
        printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
        menu();
        scanf("%d", &menu_choice);

        switch (menu_choice)
        {
        case 1:
            printf("Enter desired temperature: \n");
            scanf("%d", &input);
            send_temperature(input);
            recv_reply();
            break;
        case 2:
            printf("Enter desired humidity:\n");
            scanf("%d", &input);
            send_humidity(input);
            recv_reply();
            break;
        case 0:
            printf("bye\n");
            break;

        default:
            printf("Wrong input! Try again\n");
            break;
        }
    } while (menu_choice != 0);
    
    
    
}