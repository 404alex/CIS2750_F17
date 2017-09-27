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
    printf(printError(errorCode));
    free(string);
    return 0;
}