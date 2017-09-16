#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "CalendarParser.h"

int main() {
    char *string = malloc(sizeof(char) * 100);
    strcpy(string, "aaa.ics");
    ErrorCode errorCode = createCalendar(string, NULL);
    if (errorCode == INV_FILE) {
        printf("fail.");
    }
    free(string);
    return 0;
}