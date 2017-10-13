#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <regex.h>
#include "CalendarParser.h"
#include "help.h"

int main() {
    bool flag = true;
    char input[100];
    char longInput[10000];
    Calendar *obj = NULL;
    ErrorCode error;
    Property *tempProperty;
    char *tempDisplay;
    bool fileError = false;
    bool inputError = false;
    regex_t floatRegex;
    char *floatPattern = "^[1-9]+[\\.]?[0-9]*$";
    regex_t onlySpaceRegex;
    char *onlySpacePattern = "^[ ]*$";
    regex_t numberOnlyRegex;
    char *numOnlyPattern = "^[0-9]+$";
    Event *tempEvent;
    Alarm *tempAlarm;
    regcomp(&floatRegex, floatPattern, REG_EXTENDED);
    regcomp(&onlySpaceRegex, onlySpacePattern, REG_EXTENDED);
    regcomp(&numberOnlyRegex, numOnlyPattern, REG_EXTENDED);
    do {
        printf("**************Assignment2 Menu****************\n");
        printf("\t1. Read a icalendar file\n");
        printf("\t2. Display the file\n");
        printf("\t3. Create a new Calendar object\n");
        printf("\t4. Save the new Calendar object to file\n");
        printf("\t5. Exit\n");
        printf("***************End of Menu********************\n");
        printf("Please input menu number and hit enter: ");
        scanf("%s", input);
        if (strlen(input) != 1 || input[0] <= '0' || input[0] > '5') {
            printf("Input error, please try again!!\n");
            sleep(2);
            continue;
        }
        switch (input[0]) {
            case '1':
                if (obj != NULL) {
                    do {
                        printf("Already object in memory, do you want to continue to overwrite it?(y/n): ");
                        inputError = false;
                        scanf("%s", input);
                        if (strlen(input) != 1 || (input[0] != 'y' && input[0] != 'n')) {
                            printf("Input error, please try again!!\n");
                            inputError = true;
                        }
                    } while (inputError);
                    if (input[0] == 'n') {
                        printf("Jump to main menu\n");
                        sleep(1);
                        break;
                    } else {
                        deleteCalendar(obj);
                    }
                }
                do {
                    fileError = false;
                    printf("Please input file path, including file extension: ");
                    scanf("%s", longInput);
                    error = createCalendar(longInput, &obj);
                    if (error == INV_FILE) {
                        do {
                            printf("File error, want to re-enter file name (y/n)?");
                            inputError = false;
                            scanf("%s", input);
                            if (strlen(input) != 1 || (input[0] != 'y' && input[0] != 'n')) {
                                printf("Input error, please try again!!\n");
                                inputError = true;
                            }
                        } while (inputError);
                        if (input[0] == 'y') {
                            fileError = true;
                        }
                    } else {
                        printf(printError(error));
                        printf("\n");
                    }
                } while (fileError);
                sleep(2);
                break;
            case '2':
                if (obj == NULL) {
                    printf("Nothing in memory, can not display anything\n\n");
                    sleep(2);
                    break;
                }
                tempDisplay = printCalendar(obj);
                printf(tempDisplay);
                free(tempDisplay);
                sleep(2);
                break;
            case '3':
                if (obj != NULL) {
                    do {
                        printf("Already object in memory, do you want to continue to overwrite it?(y/n): ");
                        inputError = false;
                        scanf("%s", input);
                        if (strlen(input) != 1 || (input[0] != 'y' && input[0] != 'n')) {
                            printf("Input error, please try again!!\n");
                            inputError = true;
                        }
                    } while (inputError);
                    if (input[0] == 'n') {
                        printf("Jump to main menu\n");
                        sleep(1);
                        break;
                    } else {
                        deleteCalendar(obj);
                    }
                }
                obj = (Calendar *) malloc(sizeof(Calendar));
                obj->events = initializeList(&printEvent, &deleteEvent, &compareEvent);
                obj->properties = initializeList(&printProperties, &deleteProperties, &compareProperties);
                do {
                    inputError = false;
                    printf("\nPlease input Calender version: ");
                    scanf("%s", longInput);
                    if (regexec(&floatRegex, longInput, 0, NULL, 0) == REG_NOMATCH ||
                        longInput[strlen(longInput) - 1] == '.') {
                        printf("Error input, please input float number\n");
                        inputError = true;
                    }
                } while (inputError);
                obj->version = strtof(longInput, NULL);
                do {
                    inputError = false;
                    printf("\nPlease input Calender prodid: ");
                    scanf("%s", longInput);
                    if (regexec(&onlySpaceRegex, longInput, 1, NULL, 0) != REG_NOMATCH) {
                        printf("Error input, please do not only input white space\n");
                        inputError = true;
                    }
                } while (inputError);
                strcpy(obj->prodID, longInput);
                tempEvent = (Event *) malloc(sizeof(Event));
                tempEvent->properties = initializeList(&printProperties, &deleteProperties, &compareProperties);
                tempEvent->alarms = initializeList(&printAlarm, &deleteAlarm, &comparAlarm);
                do {
                    inputError = false;
                    printf("\nPlease input event create date (8 digits): ");
                    scanf("%s", longInput);
                    if (regexec(&numberOnlyRegex, longInput, 0, NULL, 0) == REG_NOMATCH || strlen(longInput) != 8 ||
                        atoi(longInput) <= 10000000) {
                        printf("Error input, please do only input 8 number, and can not start with 0\n");
                        inputError = true;
                    }
                } while (inputError);
                strcpy(tempEvent->creationDateTime.date, longInput);
                do {
                    inputError = false;
                    printf("\nPlease input event create time (6 digits): ");
                    scanf("%s", longInput);
                    if (regexec(&numberOnlyRegex, longInput, 0, NULL, 0) == REG_NOMATCH || strlen(longInput) != 6 ||
                        atoi(longInput) <= 100000) {
                        printf("Error input, please do only input 6 number, and can not start with 0\n");
                        inputError = true;
                    }
                } while (inputError);
                strcpy(tempEvent->creationDateTime.time, longInput);
                do {
                    inputError = false;
                    printf("\nPlease input event create time is UTC or NOT(y/n): ");
                    scanf("%s", longInput);
                    if (strlen(longInput) != 1 || (longInput[0] != 'y' && longInput[0] != 'n')) {
                        printf("Error input, please input y or n lower case\n");
                        inputError = true;
                    }
                } while (inputError);
                if (longInput[0] == 'y') {
                    tempEvent->creationDateTime.UTC = true;
                } else {
                    tempEvent->creationDateTime.UTC = false;
                }
                do {
                    inputError = false;
                    printf("\nPlease input event UID: ");
                    scanf("%s", longInput);
                    if (regexec(&onlySpaceRegex, longInput, 1, NULL, 0) != REG_NOMATCH) {
                        printf("Error input, please do not only input white space\n");
                        inputError = true;
                    }
                } while (inputError);
                strcpy(tempEvent->UID, longInput);
                tempAlarm = (Alarm *) malloc(sizeof(Alarm));
                tempAlarm->properties = initializeList(&printProperties, &deleteProperties, &compareProperties);
                do {
                    inputError = false;
                    printf("\nPlease choose alarm action Type (1. audio 2. display 3. email): ");
                    scanf("%s", longInput);
                    if (regexec(&numberOnlyRegex, longInput, 0, NULL, 0) == REG_NOMATCH || strlen(longInput) != 1 ||
                        atoi(longInput) < 1 || atoi(longInput) > 3) {
                        printf("Error input retry\n");
                        inputError = true;
                    }
                } while (inputError);
                if (longInput[0] == '1') {
                    strcpy(tempAlarm->action, "AUDIO");
                } else if (longInput[0] == '2') {
                    strcpy(tempAlarm->action, "DISPLAY");
                    do {
                        inputError = false;
                        printf("\nPlease input alarm description: ");
                        scanf("%s", longInput);
                        if (regexec(&onlySpaceRegex, longInput, 1, NULL, 0) != REG_NOMATCH) {
                            printf("Error input, please do not only input white space\n");
                            inputError = true;
                        }
                    } while (inputError);
                    tempProperty = (Property *) malloc(sizeof(Property) + (strlen(longInput) + 10) * sizeof(char));
                    strcpy(tempProperty->propName, "DESCRIPTION");
                    strcpy(tempProperty->propDescr, longInput);
                    insertBack(&(tempAlarm->properties),tempProperty);
                } else {
                    strcpy(tempAlarm->action, "EMAIL");
                    do {
                        inputError = false;
                        printf("\nPlease input alarm description: ");
                        scanf("%s", longInput);
                        if (regexec(&onlySpaceRegex, longInput, 1, NULL, 0) != REG_NOMATCH) {
                            printf("Error input, please do not only input white space\n");
                            inputError = true;
                        }
                    } while (inputError);
                    tempProperty = (Property *) malloc(sizeof(Property) + (strlen(longInput) + 10) * sizeof(char));
                    strcpy(tempProperty->propName, "DESCRIPTION");
                    strcpy(tempProperty->propDescr, longInput);
                    insertBack(&(tempAlarm->properties),tempProperty);
                    do {
                        inputError = false;
                        printf("\nPlease input alarm summary: ");
                        scanf("%s", longInput);
                        if (regexec(&onlySpaceRegex, longInput, 1, NULL, 0) != REG_NOMATCH) {
                            printf("Error input, please do not only input white space\n");
                            inputError = true;
                        }
                    } while (inputError);
                    tempProperty = (Property *) malloc(sizeof(Property) + (strlen(longInput) + 10) * sizeof(char));
                    strcpy(tempProperty->propName, "SUMMARY");
                    strcpy(tempProperty->propDescr, longInput);
                    insertBack(&(tempAlarm->properties),tempProperty);
                    do {
                        inputError = false;
                        printf("\nPlease input alarm attendee: ");
                        scanf("%s", longInput);
                        if (regexec(&onlySpaceRegex, longInput, 1, NULL, 0) != REG_NOMATCH) {
                            printf("Error input, please do not only input white space\n");
                            inputError = true;
                        }
                    } while (inputError);
                    tempProperty = (Property *) malloc(sizeof(Property) + (strlen(longInput) + 10) * sizeof(char));
                    strcpy(tempProperty->propName, "ATTENDEE");
                    strcpy(tempProperty->propDescr, longInput);
                    insertBack(&(tempAlarm->properties),tempProperty);
                }
                do {
                    inputError = false;
                    printf("\nPlease input alarm trigger: ");
                    scanf("%s", longInput);
                    if (regexec(&onlySpaceRegex, longInput, 1, NULL, 0) != REG_NOMATCH) {
                        printf("Error input, please do not only input white space\n");
                        inputError = true;
                    }
                } while (inputError);
                tempAlarm->trigger = (char *) malloc(sizeof(char) * (strlen(longInput) + 5));
                strcpy(tempAlarm->trigger, longInput);
                insertBack(&(tempEvent->alarms), tempAlarm);
                insertBack(&(obj->events), tempEvent);
                if ((error = validateCalendar(obj)) != OK) {
                    printf(printError(error));
                    deleteCalendar(obj);
                } else {
                    printf("Create success\n");
                }
                break;
            case '4':
                if (obj == NULL) {
                    printf("Nothing in memory, can not save anything\n\n");
                    sleep(2);
                    break;
                }
                do {
                    fileError = false;
                    printf("Please input file name, including file extension: ");
                    scanf("%s", longInput);

                    if (!fileNameCheck(longInput)) {
                        do {
                            printf("File name error, want to re-enter file name (y/n)?");
                            inputError = false;
                            scanf("%s", input);
                            if (strlen(input) != 1 || (input[0] != 'y' && input[0] != 'n')) {
                                printf("Input error, please try again!!\n");
                                inputError = true;
                            }
                        } while (inputError);
                        if (input[0] == 'y') {
                            fileError = true;
                        }
                    } else {
                        FILE *file = fopen(longInput, "r");
                        if (file != NULL) {
                            do {
                                printf("Already file in disk, do you want to continue to overwrite it?(y/n): ");
                                inputError = false;
                                scanf("%s", input);
                                if (strlen(input) != 1 || (input[0] != 'y' && input[0] != 'n')) {
                                    printf("Input error, please try again!!\n");
                                    inputError = true;
                                }
                            } while (inputError);
                            if (input[0] == 'n') {
                                printf("Jump to main menu\n");
                                sleep(1);
                                fclose(file);
                                break;
                            } else {
                                fclose(file);
                                error = writeCalendar(longInput, obj);
                                printf(printError(error));
                            }
                        } else {
                            error = writeCalendar(longInput, obj);
                            printf(printError(error));
                        }

                    }
                } while (fileError);
                break;
            case '5':
                flag = false;
                if (obj != NULL) {
                    deleteCalendar(obj);
                }
                if (tempDisplay != NULL) {
                    free(tempDisplay);
                }
                regfree(&floatRegex);
                regfree(&onlySpaceRegex);
                regfree(&numberOnlyRegex);
                printf("Thank you for using...");
                break;
            default:
                break;
        }

    } while (flag);
    return 0;
}