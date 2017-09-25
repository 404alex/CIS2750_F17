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
#include "../include/help.h"

char *printToken(void *toBePrinted) {

    printf("\n\n%s\n\n", toBePrinted);
}

int compartToken(const void *first, const void *second) {
    return strcmp(first, second);
}

void deleteToken(void *toBeDeleted) {
    free(toBeDeleted);
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
    ErrorCode response = fileValidation(listOfToken);
    if (response != OK) {
        clearList(&listOfToken);
        free(icsFile);
        fclose(calFile);
        return response;
    }

    ListIterator iter = createIterator(listOfToken);
    void *elem;
    while ((elem = nextElement(&iter)) != NULL) {
        char *str = listOfToken.printData(elem);
        //printf("%s\n", str);
        //free(str);
    }


    clearList(&listOfToken);
    free(icsFile);
    fclose(calFile);

}

void deleteCalendar(Calendar *obj) {

}

char *printCalendar(const Calendar *obj) {

}

const char *printError(ErrorCode err) {

}