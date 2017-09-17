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
#include "../include/CalendarParser.h"

//already tested, no memory leak, no error.
char *stringToLower(char *string) {
    for (int i = 0; i < strlen(string); ++i) {
        string[i] = tolower(string[i]);
    }
    return string;
}

//already tested, no memory leak, no error.
bool fileNameCheck(char *fileName) {
    if (fileName == NULL) {
        return false;
    }
    if (strlen(fileName) == 0) {
        return false;
    }
    int strLength = strlen(fileName);
    int startPoint = strLength - 4;
    if (strLength <= 4) {
        return false;
    }
    char *extension = malloc(sizeof(char) * 5);
    strncpy(extension, fileName + startPoint, 5);
    extension = stringToLower(extension);
    if (strcmp(extension, ".ics") == 0) {
        free(extension);
        return true;
    } else {
        free(extension);
        return false;
    }
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


    fclose(calFile);

}

void deleteCalendar(Calendar *obj) {

}

char *printCalendar(const Calendar *obj) {

}

const char *printError(ErrorCode err) {

}