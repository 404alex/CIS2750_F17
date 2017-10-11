#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "CalendarParser.h"
#include "help.h"

int main() {
    char *string = malloc(sizeof(char) * 100);
    strcpy(string, "test.ics");
    Calendar *obj = NULL;
    ErrorCode error = createCalendar(string, &obj);
    printf(printError(error));

    printf(printError(validateCalendar(obj)));

    free(string);
    error = writeCalendar("test.ics", obj);
    printf(printError(error));
    deleteCalendar(obj);
    return 0;
}