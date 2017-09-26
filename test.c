#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "CalendarParser.h"
#include "help.h"

int main() {
    char *string = malloc(sizeof(char) * 100);
    strcpy(string, "aaa.ics");
    ErrorCode errorCode = createCalendar(string, NULL);
    switch(errorCode)
    {
        case OK:
            printf("OK");
            break;
        case INV_EVENT:
            printf("INV_EVENT");
            break;
        case INV_CAL:
            printf("INV_CAL");
            break;
        case INV_VER:
            printf("INV_VER");
            break;
        case INV_FILE:
            printf("INV_FILE");
            break;
        case INV_CREATEDT:
            printf("INV_CREATEDT");
            break;
        case INV_PRODID:
            printf("INV_PRODID");
            break;
        case DUP_VER:
            printf("DUP_VER");
            break;
        case DUP_PRODID:
            printf("DUP_PRODID");
            break;
        default:
            printf("default");
            break;
    }
    free(string);
    return 0;
}