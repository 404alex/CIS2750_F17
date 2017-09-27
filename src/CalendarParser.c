/**
 * CIS2750 F2017
 * Assignment 1
 * Chenxingyu Chen 0951136
 * Description:
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "CalendarParser.h"
#include "LinkedListAPI.h"
#include "help.h"

char *printToken(void *toBePrinted) {

    printf("\n\n%s\n\n", toBePrinted);
}

int compartToken(const void *first, const void *second) {
    return strcmp(first, second);
}

void deleteToken(void *toBeDeleted) {
    free(toBeDeleted);
}

char *printAlarm(void *toBePrinted) {
    Alarm *alarm = (Alarm *) toBePrinted;
    ListIterator iter = createIterator(alarm->properties);
    char *property = malloc(sizeof(char) * 10);
    strcpy(property, "");
    void *elem;
    while ((elem = nextElement(&iter)) != NULL) {
        char *temp = alarm->properties.printData(elem);
        property = realloc(property, sizeof(char) * (strlen(property) + strlen(temp) + 7));
        strcat(property, temp);
        strcat(property, "\n");
        free(temp);
    }
    char *string = malloc()
}

int comparAlarm(const void *first, const void *second) {
    if (first == NULL || second == NULL) {
        return 0;
    }
    Alarm *alarm1 = (Alarm *) first;
    Alarm *alarm2 = (Alarm *) second;
    return strcmp(alarm1->trigger, alarm2->trigger);
}

void deleteAlarm(void *toBeDeleted) {
    Alarm *alarm = (Alarm *) toBeDeleted;
    free(alarm->trigger);
    clearList(&alarm->properties);
}

char *printProperties(void *toBePrinted) {
    Property *aProperty = (Property *) toBePrinted;
    int length = strlen(aProperty->propName) + strlen(aProperty->propDescr) + 5;
    char *string = malloc(sizeof(char) * length);
    strcpy(string, aProperty->propName);
    strcat(string, ":");
    strcat(string, aProperty->propDescr);
    return string;
}

int compareProperties(const void *first, const void *second) {
    if (first == NULL || second == NULL) {
        return 0;
    }
    Property *Property1 = (Property *) first;
    Property *Property2 = (Property *) second;
    return strcmp(Property1->propName, Property2->propName);

}

void deleteProperties(void *toBeDeleted) {
    Property *aProperty = (Property *) toBeDeleted;
    free(aProperty);
}


ErrorCode createCalendar(char *fileName, Calendar **obj) {
    if (!fileNameCheck(fileName)) {
        return INV_FILE;
    }
    FILE *calFile = fopen(fileName, "r");
    if (!calFile) {
        perror("File opening failed.");
        return INV_FILE;
    }
    char *icsFile = readIntoBuffer(calFile);
    List listOfToken = initializeList(&printToken, &deleteToken, &compartToken);
    insertTokenizedList(icsFile, &listOfToken);
    if (!unFoldData(&listOfToken)) {
        clearList(&listOfToken);
        free(icsFile);
        fclose(calFile);
        return INV_CAL;
    }
    deleteCommont(&listOfToken);

    ErrorCode response = fileValidation(listOfToken);
    if (response != OK) {
        clearList(&listOfToken);
        free(icsFile);
        fclose(calFile);
        return response;
    }
    int countOfVCAL = countCalObject(&listOfToken);
    if (countOfVCAL == 0) {
        clearList(&listOfToken);
        free(icsFile);
        fclose(calFile);
        return INV_CAL;
    }
    int index = 0;
    obj = malloc(sizeof(Calendar *) * countOfVCAL);
    ListIterator iter = createIterator(listOfToken);
    void *elem;
    while ((elem = nextElement(&iter)) != NULL) {
        switch (contentIndicator(elem)) {
            case 0:
                obj[index]->event = malloc(sizeof(Event));
                obj[index]->event->properties = initializeList(&printProperties, &deleteProperties, &compareProperties);
                obj[index]->event->alarms = initializeList(&printAlarm, &deleteAlarm, &comparAlarm);

            case 1:
                index++;
                break;
        }
    }

    clearList(&listOfToken);
    free(icsFile);
    fclose(calFile);
    return OK;

}

void deleteCalendar(Calendar *obj) {
    Event *tempEvent = obj->event;
    clearList(&tempEvent->properties);
    clearList(&tempEvent->alarms);
    free(tempEvent);
    free(obj);
}

char *printCalendar(const Calendar *obj) {

}

const char *printError(ErrorCode err) {
    switch (err) {
        case OK:
            return "Create Success";
        case INV_EVENT:
            return "Event Component is invalid. (No event, invalid opening closing tags. Event do not have uid. Event do not have stamp time. Event do not have valid start time end time pair.)";
        case INV_CAL:
            return "Calendar it self is invalid. (Invalid opening closing tags. Event outside calender. No calender in the file.)";
        case INV_VER:
            return "Version is invalid (Version not under calender block. Calender have no version property. Version number have invalid format.)";
        case INV_FILE:
            return "File is invalid (File argument is NULL, empty, or do not contain .ics extension. File can not be opened. File does not exist.)";
        case INV_CREATEDT:
            return "Event creation date-time error. (Event does not contain valid creation date-time. Event creation date-time at wrong place.)";
        case INV_PRODID:
            return "Product id is invalid. (Calendar do not have a product id. Product id put at wrong place.)";
        case DUP_VER:
            return "One calendar object has more than one version number.";
        case DUP_PRODID:
            return "One calendar object has more than one product id.";
        case OTHER_ERROR:
            return "Some Unknow error. (Do not have enought memory to allocate)";
        default:
            return "Nothing";
    }
}