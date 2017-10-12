#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "CalendarParser.h"
#include "help.h"

int main() {
    printf("*****Assignment2 Menu********\n");
    printf("\t1. Read a icalendar file\n");
    printf("\t2. Display the file");
    printf("\t3. Create a new Calendar object\n");
    printf("\t4. Save the new Calendar object to file\n");
    printf("\t5. Exit\n");
    printf("*****End of Menu********\n");





    char *string = malloc(sizeof(char) * 100);
    strcpy(string, "testCalEvtPropAlm.ics");
    Calendar *obj = NULL;
    ErrorCode error = createCalendar(string, &obj);
    printf(printError(error));

//    printf(printError(validateCalendar(obj)));
//
    free(string);
//    error = writeCalendar("test.ics", obj);
//    printf(printError(error));
//    deleteCalendar(obj);
    return 0;
}