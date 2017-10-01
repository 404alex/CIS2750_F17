#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "CalendarParser.h"
#include "help.h"

int main() {
    char *string = malloc(sizeof(char) * 100);
    strcpy(string, "testCalShort.ics");
    Calendar *obj;
    createCalendar(string, &obj);
    char *printInfo = printCalendar(obj);
    printf(printInfo);
    free(printInfo);
    free(&obj);
    deleteCalendar(obj);
    free(string);
    return 0;
}