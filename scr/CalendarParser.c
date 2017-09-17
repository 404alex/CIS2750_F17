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
#include "../include/LinkedListAPI.h"

char *printCalenderObjFunc(void *toBePrinted) {
    return NULL;
}

int compareCalenderObjFunc(const void *first, const void *second) {
    return 0;
}

void deleteCalenderObjFunc(void *toBeDeleted) {
    free(toBeDeleted);
}


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

/**
 * http://blog.csdn.net/zdyueguanyun/article/details/51392736
 * @param file file point
 * @return the length of the file.
 */
int fileSize(FILE *file) {
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);
    return size;
}

/**
 * read the whole file and return a string, the string is dynamic allocate, so need free!
 * @param file file point
 * @return the string of whole file, need free.
 */
char *readIntoBuffer(FILE *file) {
    char *fileBuffer = malloc(sizeof(char) * (fileSize(file) + 2));
    strcpy(fileBuffer, "");
    char temp[100];
    while ((fgets(temp, 100, file)) != NULL) {
        strcat(fileBuffer, temp);
    }
    return fileBuffer;
}

char *unFlodICSFile(char *icsFile) {
    char *temp = icsFile;
    while (*temp) {
        if (strncpy(temp, "\n ", 2) == 0) {

        }
        temp++;
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
    char *icsFile = readIntoBuffer(calFile);
    List list = initializeList(&printCalenderObjFunc, &deleteDataFromList, &compareCalenderObjFunc);


    free(icsFile);
    fclose(calFile);

}

void deleteCalendar(Calendar *obj) {

}

char *printCalendar(const Calendar *obj) {

}

const char *printError(ErrorCode err) {

}