#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "CalendarParser.h"
#include "help.h"

int main() {
    char *string = malloc(sizeof(char) * 100);
    strcpy(string, "testCalEvtPropAlm.ics");
    Calendar *obj = NULL;
    ErrorCode error = createCalendar(string, &obj);
    printf(printError(error));
    if (error == OK) {
        char *printInfo = printCalendar(obj);
        printf(printInfo);
        free(printInfo);
    }

    free(string);
    ErrorCode error1 = validateCalendar(obj);
    printf(printError(error1));
    deleteCalendar(obj);
    return 0;
}