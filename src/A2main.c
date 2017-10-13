#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "CalendarParser.h"
#include "help.h"

int main() {
    bool flag = true;
    char input[100];
    char path[10000];
    Calendar *obj = NULL;
    ErrorCode error;
    char *tempDisplay;
    bool fileError = false;
    bool inputError = false;
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
                    scanf("%s", path);
                    error = createCalendar(path, &obj);
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
                break;
            case '4':
                break;
            case '5':
                flag = false;
                if (obj != NULL) {
                    deleteCalendar(obj);
                }
                if (tempDisplay != NULL) {
                    free(tempDisplay);
                }
                printf("Thank you for using...");
                break;
            default:
                break;
        }

    } while (flag);
//    char *string = malloc(sizeof(char) * 100);
//    strcpy(string, "testCalEvtPropAlm.ics");
//    Calendar *obj = NULL;
//    ErrorCode error = createCalendar(string, &obj);
//    printf(printError(error));
//
////    printf(printError(validateCalendar(obj)));
////
//    free(string);
//    error = writeCalendar("test.ics", obj);
//    printf(printError(error));
//    deleteCalendar(obj);
    return 0;
}