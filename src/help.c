#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "help.h"
#include "LinkedListAPI.h"
#include "CalendarParser.h"
#include <regex.h>


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

int fileSize(FILE *file) {
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);
    return size;
}


char *readIntoBuffer(FILE *file) {
    char *fileBuffer = malloc(sizeof(char) * (fileSize(file) + 2));
    strcpy(fileBuffer, "");
    char temp[100];
    while ((fgets(temp, 100, file)) != NULL) {
        strcat(fileBuffer, temp);
    }
    return fileBuffer;
}

/**
 * http://blog.csdn.net/dezhihuang/article/details/49669193
 * tokenize string by some charaters
 * @param str the string which need to be tokenized
 * @param c the delimit characters
 * @param startPoint the start point of the tokenze, the first time should put 0
 * @return string that after tokenize.
 */
char *tokenize(const char *str, const char c, int *const startPoint) {
    int length = strlen(str);
    if (*startPoint >= length) {
        return "ERROREND";
    }
    char *afterToken = malloc(sizeof(char) * (length + 2 - *startPoint));
    strcpy(afterToken, "");
    int i = 0;
    for (i = *startPoint; i < length + 1; ++i) {
        if (str[i] != c) {
            afterToken[i - *startPoint] = str[i];
        } else {
            break;
        }
    }
    afterToken[i - *startPoint] = '\0';
    *startPoint = i + 1;
    return afterToken;
}

char *unFlodICSFile(char *icsFile) {
    char *temp = icsFile;
    while (*temp) {
        if (strncpy(temp, "\n ", 2) == 0) {

        }
        temp++;
    }
}

void insertTokenizedList(const char *icsFile, List *listOfToken) {
    int startPoint = 0;
    char *afterToken;
    do {
        afterToken = tokenize(icsFile, '\n', &startPoint);
        if (strcmp(afterToken, "\n") == 0) {
            free(afterToken);
            afterToken = "";
            continue;
        }
        if (strcmp(afterToken, "") == 0) {
            free(afterToken);
            afterToken = "";
            continue;
        }
        if (strcmp(afterToken, "ERROREND") == 0) {
            break;
        }
        if (strcmp(afterToken, "")) {
            insertBack(listOfToken, afterToken);
        }
    } while (strcmp(afterToken, "ERROREND"));
}

bool unFoldData(List *listOfToken) {
    ListIterator iter = createIterator(*listOfToken);
    void *elem;
    while ((elem = nextElement(&iter)) != NULL) {
        if (strncmp(" ", elem, 1) == 0) {
            if (iter.current == NULL) {
                return false;
            }
            char *string = deleteDataFromList(listOfToken, elem);
            if (iter.current->previous == NULL) {
                free(string);
                return false;
            }
            char *previousData = iter.current->previous->data;
            int size = strlen(previousData) + strlen(string) + 5;
            char *newData = malloc(sizeof(char) * size);
            strcpy(newData, previousData);
            strcat(newData, string + 1);
            iter.current->previous->data = newData;
            listOfToken->deleteData(previousData);
            free(string);
        }
    }
    return true;
}


ErrorCode fileValidation(List listOfToken) {
    int vcaleCount = 0;
    int calenderCount = 0;
    int veventCount = 0;
    int eventCount = 0;
    int versionCount = 0;
    int prodidCount = 0;
    int uidCount = 0;
    int dtStampCount = 0;
    int endTimeCount = 0;
    int dtstartCount = 0;
    int dtendCount = 0;
    regex_t beginCalRegex;
    regex_t endCalRegex;
    regex_t beginEveRegex;
    regex_t endEveRegex;
    regex_t versionRegex;
    regex_t proidRegex;
    regex_t uidRegex;
    regex_t dtStampRegex;
    regex_t dtstartRegex;
    regex_t dtendRegex;
    regex_t durationRegex;
    char *beginCalPattern = "^[Bb][Ee][Gg][Ii][Nn]:[ ]*[Vv][Cc][Aa][Ll][Ee][Nn][Dd][Aa][Rr]$";
    char *endCalPattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Cc][Aa][Ll][Ee][Nn][Dd][Aa][Rr]$";
    char *beginEvePattern = "^[Bb][Ee][Gg][Ii][Nn]:[ ]*[Vv][Ee][Vv][Ee][Nn][Tt]$";
    char *endEvePattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Ee][Vv][Ee][Nn][Tt]$";
    char *versionPattern = "^[Vv][Ee][Rr][Ss][Ii][Oo][Nn]:[ ]*[0-9]+\\.?[0-9]+$";
    char *proidPattern = "^[Pp][Rr][Oo][Dd][Ii][Dd]:[ ]*[0-9A-Za-z/\\\\\\.\\-]{1}.*";
    char *uidPattern = "^[Uu][Ii][Dd]:[ ]*[0-9A-Za-z/\\\\\\.\\-]{1}.*";
    char *dtStampPattern = "[Dd][Tt][Ss][Tt][Aa][Mm][Pp]:[ ]*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[A-Za-z]{1}$";
    char *dtstartPattern = "^[Dd][Tt][Ss][Tt][Aa][Rr][Tt]:[ ]*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[A-Za-z]{1}$";
    char *dtendPattern = "^[Dd][Tt][Ee][Nn][Dd]:[ ]*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[A-Za-z]{1}$";
    char *durationPattern = "[Dd][Uu][Rr][Aa][Tt][Ii][Oo][Nn]:[ ]*[Pp][Tt][0-9]*[Hh]?[0-9]*[Mm]?[0-9]*[Ss]?$";
    regcomp(&endCalRegex, endCalPattern, REG_NOSUB);
    regcomp(&beginCalRegex, beginCalPattern, REG_NOSUB);
    regcomp(&beginEveRegex, beginEvePattern, REG_NOSUB);
    regcomp(&endEveRegex, endEvePattern, REG_NOSUB);
    regcomp(&versionRegex, versionPattern, REG_EXTENDED);
    regcomp(&proidRegex, proidPattern, REG_EXTENDED);
    regcomp(&uidRegex, uidPattern, REG_EXTENDED);
    regcomp(&dtStampRegex, dtStampPattern, REG_EXTENDED);
    regcomp(&dtstartRegex, dtstartPattern, REG_EXTENDED);
    regcomp(&dtendRegex, dtendPattern, REG_EXTENDED);
    regcomp(&durationRegex, durationPattern, REG_EXTENDED);


    ListIterator iter = createIterator(listOfToken);
    void *elem;
    while ((elem = nextElement(&iter)) != NULL) {
        //VCAL verified.
        if (regexec(&beginCalRegex, elem, NULL, NULL, 0) != REG_NOMATCH) {
            vcaleCount++;
            calenderCount++;
        }
        if (regexec(&endCalRegex, elem, NULL, NULL, 0) != REG_NOMATCH) {
            vcaleCount--;
        }
        if (vcaleCount < 0 || vcaleCount > 1) {
            regfree(&beginCalRegex);
            regfree(&endCalRegex);
            regfree(&beginEveRegex);
            regfree(&endEveRegex);
            regfree(&versionRegex);
            regfree(&proidRegex);
            regfree(&uidRegex);
            regfree(&dtStampRegex);
            regfree(&dtstartRegex);
            regfree(&dtendRegex);
            regfree(&durationRegex);
            return INV_CAL;
        }
        //vevent verified
        if (regexec(&beginEveRegex, elem, NULL, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1) {
                regfree(&beginCalRegex);
                regfree(&endCalRegex);
                regfree(&beginEveRegex);
                regfree(&endEveRegex);
                regfree(&versionRegex);
                regfree(&proidRegex);
                regfree(&uidRegex);
                regfree(&dtStampRegex);
                regfree(&dtstartRegex);
                regfree(&dtendRegex);
                regfree(&durationRegex);

                return INV_EVENT;
            }
            veventCount++;
            eventCount++;
        }
        if (regexec(&endEveRegex, elem, NULL, NULL, 0) != REG_NOMATCH) {
            veventCount--;
        }
        if (veventCount < 0 || veventCount > 1) {
            regfree(&beginCalRegex);
            regfree(&endCalRegex);
            regfree(&beginEveRegex);
            regfree(&endEveRegex);
            regfree(&versionRegex);
            regfree(&proidRegex);
            regfree(&uidRegex);
            regfree(&dtStampRegex);
            regfree(&dtstartRegex);
            regfree(&dtendRegex);
            regfree(&durationRegex);


            return INV_EVENT;
        }


        //version verified
        if (regexec(&versionRegex, elem, NULL, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1 || veventCount != 0) {
                regfree(&beginCalRegex);
                regfree(&endCalRegex);
                regfree(&beginEveRegex);
                regfree(&endEveRegex);
                regfree(&versionRegex);
                regfree(&proidRegex);
                regfree(&uidRegex);
                regfree(&dtStampRegex);
                regfree(&dtstartRegex);
                regfree(&dtendRegex);
                regfree(&durationRegex);

                return INV_VER;
            }
            versionCount++;
        }

        //prodid verified
        if (regexec(&proidRegex, elem, NULL, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1 || veventCount != 0) {
                regfree(&beginCalRegex);
                regfree(&endCalRegex);
                regfree(&beginEveRegex);
                regfree(&endEveRegex);
                regfree(&versionRegex);
                regfree(&proidRegex);
                regfree(&uidRegex);
                regfree(&dtStampRegex);
                regfree(&dtstartRegex);
                regfree(&dtendRegex);
                regfree(&durationRegex);

                return INV_PRODID;
            }
            prodidCount++;
        }

        //UID verified
        if (regexec(&uidRegex, elem, NULL, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1 || veventCount != 1) {
                regfree(&beginCalRegex);
                regfree(&endCalRegex);
                regfree(&beginEveRegex);
                regfree(&endEveRegex);
                regfree(&versionRegex);
                regfree(&proidRegex);
                regfree(&uidRegex);
                regfree(&dtStampRegex);
                regfree(&dtstartRegex);
                regfree(&dtendRegex);
                regfree(&durationRegex);

                return INV_EVENT;
            }
            uidCount++;
        }


        //created time verified
        if (regexec(&dtStampRegex, elem, NULL, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1 || veventCount != 1) {
                regfree(&beginCalRegex);
                regfree(&endCalRegex);
                regfree(&beginEveRegex);
                regfree(&endEveRegex);
                regfree(&versionRegex);
                regfree(&proidRegex);
                regfree(&uidRegex);
                regfree(&dtStampRegex);
                regfree(&dtstartRegex);
                regfree(&dtendRegex);
                regfree(&durationRegex);

                return INV_EVENT;
            }
            dtStampCount++;
        }

        if (regexec(&dtendRegex, elem, NULL, NULL, 0) != REG_NOMATCH ||
            regexec(&durationRegex, elem, NULL, NULL, 0) != REG_NOMATCH) {
            dtendCount++;
        }
        //dtstart Verification
        if (regexec(&dtstartRegex, elem, NULL, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1 || veventCount != 1) {
                regfree(&beginCalRegex);
                regfree(&endCalRegex);
                regfree(&beginEveRegex);
                regfree(&endEveRegex);
                regfree(&versionRegex);
                regfree(&proidRegex);
                regfree(&uidRegex);
                regfree(&dtStampRegex);
                regfree(&dtstartRegex);
                regfree(&dtendRegex);
                regfree(&durationRegex);

                return INV_EVENT;
            } else {
                dtstartCount++;
                Node *p = iter.current->previous;
                while (p != NULL && regexec(&endEveRegex, p->data, NULL, NULL, 0) == REG_NOMATCH) {
                    if (regexec(&dtendRegex, p->data, NULL, NULL, 0) != REG_NOMATCH ||
                        regexec(&durationRegex, p->data, NULL, NULL, 0) != REG_NOMATCH) {
                        endTimeCount++;
                    }
                    p = p->next;
                }
                if (endTimeCount != 1) {
                    endTimeCount = 0;
                    regfree(&beginCalRegex);
                    regfree(&endCalRegex);
                    regfree(&beginEveRegex);
                    regfree(&endEveRegex);
                    regfree(&versionRegex);
                    regfree(&proidRegex);
                    regfree(&uidRegex);
                    regfree(&dtStampRegex);
                    regfree(&dtstartRegex);
                    regfree(&dtendRegex);
                    regfree(&durationRegex);
                    return INV_EVENT;
                } else {
                    endTimeCount = 0;
                }
            }
        }
    }


    regfree(&beginCalRegex);
    regfree(&endCalRegex);
    regfree(&beginEveRegex);
    regfree(&endEveRegex);
    regfree(&versionRegex);
    regfree(&proidRegex);
    regfree(&uidRegex);
    regfree(&dtStampRegex);
    regfree(&dtstartRegex);
    regfree(&dtendRegex);
    regfree(&durationRegex);

    if (vcaleCount != 0) {
        return INV_CAL;
    }
    if (veventCount != 0) {
        return INV_EVENT;
    }
    if (versionCount == 0) {
        return INV_VER;
    }
    if (prodidCount == 0) {
        return INV_PRODID;
    }
    if (versionCount != calenderCount) {
        return DUP_VER;
    }
    if (prodidCount != calenderCount) {
        return DUP_PRODID;
    }
    if (calenderCount == 0) {
        return INV_CAL;
    }
    if (eventCount == 0) {
        return INV_EVENT;
    }
    if (uidCount != eventCount) {
        return INV_EVENT;
    }
    if (dtStampCount != eventCount) {
        return INV_EVENT;
    }
    if (dtstartCount != dtendCount) {
        return INV_EVENT;
    }

    return OK;

}


void deleteCommont(List *listOfToken) {
    ListIterator iter = createIterator(*listOfToken);
    void *elem;
    while ((elem = nextElement(&iter)) != NULL) {
        if (strncmp(";", elem, 1) == 0) {
            void *deletedData = deleteDataFromList(listOfToken, elem);
            listOfToken->deleteData(deletedData);
        }
    }
}