/**
 * CIS2750 F2017
 * Assignment 1
 * Chenxingyu Chen 0951136
 * Description:
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CalendarParser.h"
#include "LinkedListAPI.h"
#include "help.h"

ICalErrorCode createCalendar(char *fileName, Calendar **obj) {
    if (fileName == NULL) {
        return INV_FILE;
    }
    if (!fileNameCheck(fileName)) {
        return INV_FILE;
    }
    FILE *calFile = fopen(fileName, "r");
    if (!calFile) {
        perror("File opening failed.");
        return INV_FILE;
    }
    char *icsFile = readIntoBuffer(calFile);
    if (strlen(icsFile) <= 52) {
        free(icsFile);
        fclose(calFile);
        return INV_CAL;
    }
    List listOfToken = initializeList(&printToken, &deleteToken, &compartToken);
    insertTokenizedList(icsFile, &listOfToken);
    if (!unFoldData(&listOfToken)) {
        clearList(&listOfToken);
        free(icsFile);
        fclose(calFile);
        return INV_CAL;
    }
    deleteCommont(&listOfToken);

    ICalErrorCode response = fileValidation(listOfToken);
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
    //*obj = (Calendar *) malloc(sizeof(Calendar *) * 1);
    ListIterator iter = createIterator(listOfToken);
    void *elem;
    char *temp;
    char *description;
    Alarm *alarmTemp;
    Property *aProperty;
    Event *event;
    float versionNumber;
    bool haveStartDT = false;
    while ((elem = nextElement(&iter)) != NULL) {
        if (contentIndicator(elem) == 1) {
            break;
        }
        switch (contentIndicator(elem)) {
            case 0:
                obj[index] = (Calendar *) malloc(sizeof(Calendar));
                obj[index]->events = initializeList(&printEvent, &deleteEvent, &compareEvent);
                obj[index]->properties = initializeList(&printProperties, &deleteProperties, &compareProperties);
                break;
            case 1:
                //index++;
                break;
            case 2:
                versionNumber = getVersionNumber(elem);
                if (versionNumber == 0) {
                    deleteCalendar(obj[index]);
                    obj[index] = NULL;
                    clearList(&listOfToken);
                    free(icsFile);
                    fclose(calFile);
                    return INV_VER;
                }
                obj[index]->version = getVersionNumber(elem);
                break;
            case 3:
                temp = getProdid(elem);
                if (temp == NULL) {
                    deleteCalendar(obj[index]);
                    obj[index] = NULL;
                    clearList(&listOfToken);
                    free(icsFile);
                    fclose(calFile);
                    return INV_PRODID;
                }
                strcpy(obj[index]->prodID, temp);
                free(temp);
                break;
            case 4:
                event = (Event *) malloc(sizeof(Event));
                event->properties = initializeList(&printProperties, &deleteProperties, &compareProperties);
                event->alarms = initializeList(&printAlarm, &deleteAlarm, &comparAlarm);
                while ((elem = nextElement(&iter)) != NULL && !isEndEvent(elem)) {
                    switch (contentIndicator(elem)) {
                        case 6:
                            temp = getUID(elem);
                            strcpy(event->UID, temp);
                            free(temp);
                            break;
                        case 7:
                            event->creationDateTime.UTC = true;
                            temp = getUTCDate(elem);
                            strcpy(event->creationDateTime.date, temp);
                            free(temp);
                            temp = getUTCTime(elem);
                            strcpy(event->creationDateTime.time, temp);
                            free(temp);
                            break;
                        case 8:
                            event->creationDateTime.UTC = false;
                            temp = getUTCDate(elem);
                            strcpy(event->creationDateTime.date, temp);
                            free(temp);
                            temp = getTime(elem);
                            strcpy(event->creationDateTime.time, temp);
                            free(temp);
                            break;
                        case 12:
                            haveStartDT = true;
                            event->startDateTime.UTC = true;
                            temp = getUTCDate(elem);
                            strcpy(event->startDateTime.date, temp);
                            free(temp);
                            temp = getUTCTime(elem);
                            strcpy(event->startDateTime.time, temp);
                            free(temp);
                            break;
                        case 13:
                            haveStartDT = true;
                            event->startDateTime.UTC = false;
                            temp = getUTCDate(elem);
                            strcpy(event->startDateTime.date, temp);
                            free(temp);
                            temp = getTime(elem);
                            strcpy(event->startDateTime.time, temp);
                            free(temp);
                            break;
                        case 9:
                            alarmTemp = (Alarm *) malloc(sizeof(Alarm));
                            alarmTemp->trigger = NULL;
                            alarmTemp->properties = initializeList(&printProperties, &deleteProperties,
                                                                   &compareProperties);
                            while ((elem = nextElement(&iter)) != NULL && !isEndAlarm(elem)) {
                                switch (contentIndicator(elem)) {
                                    case 10:
                                        temp = getAlarmAction(elem);
                                        strcpy(alarmTemp->action, temp);
                                        free(temp);
                                        break;
                                    case 11:
                                        temp = getAlarmTri(elem);
                                        alarmTemp->trigger = malloc(sizeof(char) * (strlen(temp) + 5));
                                        strcpy(alarmTemp->trigger, temp);
                                        free(temp);
                                        break;
                                    default:
                                        description = getDescription(elem);
                                        temp = getName(elem);
                                        if (temp == NULL) {
                                            continue;
                                        }
                                        if (description == NULL) {
                                            description = malloc(sizeof(char) * 2);
                                            strcpy(description, "");
                                        }
                                        aProperty = (Property *) malloc(
                                                sizeof(Property) + (strlen(description) + 2) * sizeof(char));
                                        strcpy(aProperty->propDescr, description);
                                        strcpy(aProperty->propName, temp);
                                        insertBack(&(alarmTemp->properties), aProperty);
                                        free(temp);
                                        free(description);
                                        break;
                                }
                            }
                            insertBack(&(event->alarms), alarmTemp);
                            break;
                        default:
                            description = getDescription(elem);
                            temp = getName(elem);
                            if (temp == NULL) {
                                continue;
                            }
                            if (description == NULL) {
                                description = malloc(sizeof(char) * 2);
                                strcpy(description, "");
                            }
                            aProperty = (Property *) malloc(
                                    sizeof(Property) + (strlen(description) + 2) * sizeof(char));
                            strcpy(aProperty->propDescr, description);
                            strcpy(aProperty->propName, temp);
                            insertBack(&(event->properties), aProperty);
                            free(temp);
                            free(description);
                            break;
                    }
                }
                if (haveStartDT == false) {
                    event->startDateTime.UTC = event->creationDateTime.UTC;
                    strcpy(event->startDateTime.time, event->creationDateTime.time);
                    strcpy(event->startDateTime.date, event->creationDateTime.date);
                }
                insertBack(&(obj[index]->events), event);
                break;
            default:
                description = getDescription(elem);
                temp = getName(elem);
                if (temp == NULL) {
                    continue;
                }
                if (description == NULL) {
                    description = malloc(sizeof(char) * 2);
                    strcpy(description, "");
                }
                aProperty = (Property *) malloc(
                        sizeof(Property) + (strlen(description) + 2) * sizeof(char));
                strcpy(aProperty->propDescr, description);
                strcpy(aProperty->propName, temp);
                insertBack(&(obj[index]->properties), aProperty);
                free(temp);
                free(description);
                break;
        }
    }
    free(icsFile);
    fclose(calFile);
    clearList(&listOfToken);
    ICalErrorCode errorCode;
    if ((errorCode = validateCalendar(obj[index])) != OK) {
        deleteCalendar(*obj);
        obj[index] = NULL;
        return errorCode;
    }
    return OK;
}

void deleteCalendar(Calendar *obj) {
    if (obj == NULL) { return; }
    clearList(&(obj->events));
    clearList(&(obj->properties));
    free(obj);
    obj = NULL;
}

char *printCalendar(const Calendar *obj) {
    if (obj == NULL) {
        return NULL;
    }
    char *event = malloc(sizeof(char) * 10);
    strcpy(event, "");
    ListIterator iter = createIterator(obj->events);
    void *elem;
    while ((elem = nextElement(&iter)) != NULL) {
        char *temp = obj->events.printData(elem);
        event = realloc(event, sizeof(char) * (strlen(event) + strlen(temp) + 7));
        strcat(event, temp);
        strcat(event, "\r\n");
        free(temp);
    }
    char *version = malloc(sizeof(char) * 5);
    gcvt(obj->version, 2, version);
    char *string = malloc(sizeof(char) * (strlen(event) + strlen(obj->prodID) + strlen(version) + 50));
    strcpy(string, "Calendar: version = ");
    strcat(string, version);
    strcat(string, ", prodID = ");
    strcat(string, obj->prodID);
    strcat(string, "\r\nEvent\r\n");
    strcat(string, event);
    free(event);
    free(version);
    return string;
}


/**
 * Print readable error message. return string, do not need free!
 * @param err error code, enum type.
 * @return a string which do not need to be freed.
 */
char *printError(ICalErrorCode err) {
    switch (err) {
        case OK:
            return "Allocated object\nFile parsed successfully\n";
        case INV_EVENT:
            return "Event Component is invalid. (No event, invalid opening closing tags. Event do not have uid. Event do not have stamp time. Event do not have valid start time end time pair.)\n";
        case INV_CAL:
            return "Calendar it self is invalid. (Invalid opening closing tags. Event outside calender. No calender in the file.)\n";
        case INV_VER:
            return "Version is invalid (Version not under calender block. Calender have no version property. Version number have invalid format.)\n";
        case INV_FILE:
            return "File is invalid (File argument is NULL, empty, or do not contain .ics extension. File can not be opened. File does not exist.)\n";
        case INV_CREATEDT:
            return "Event creation date-time error. (Event does not contain valid creation date-time. Event creation date-time at wrong place.)\n";
        case INV_PRODID:
            return "Product id is invalid. (Calendar do not have a product id. Product id put at wrong place.)\n";
        case DUP_VER:
            return "One calendar object has more than one version number.\n";
        case DUP_PRODID:
            return "One calendar object has more than one product id.\n";
        case OTHER_ERROR:
            return "Some Unknow error. (Do not have enought memory to allocate)\n";
        case INV_ALARM:
            return "Alarm is invalid.";
        case WRITE_ERROR:
            return "Can not write to file.";
        default:
            return "Nothing";
    }
}

ICalErrorCode writeCalendar(char *fileName, const Calendar *obj) {
    ICalErrorCode error;
    if ((error = validateCalendar(obj)) != OK) {
        return error;
    }

    if (!fileNameCheck(fileName)) {
        return INV_FILE;
    }
//    FILE *test = fopen(fileName, "r");
//    if (test != NULL) {
//        fclose(test);
//        return INV_FILE;
//    }
    //todo last time write to here;
    char *needWriteString = writeCalendarString(obj);
    needWriteString = foldWritenString(needWriteString);


    FILE *file = fopen(fileName, "w");
    int length = fwrite(needWriteString, sizeof(char), strlen(needWriteString), file);
    free(needWriteString);
    fclose(file);
    if (length <= 0) {
        return WRITE_ERROR;
    } else {
        return OK;
    }
}

ICalErrorCode validateCalendar(const Calendar *obj) {
    if (obj == NULL) {
        return INV_CAL;
    }
    ICalErrorCode error;
    if ((error = vCalValidate(obj)) != OK) {
        return error;
    }

    if ((error = vEventValidate(obj->events)) != OK) {
        return error;
    }
    return OK;
}
