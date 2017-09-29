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


void cleanRegex(regex_t *beginCalRegex, regex_t *endCalRegex, regex_t *beginEveRegex, regex_t *endEveRegex,
                regex_t *versionRegex, regex_t *proidRegex, regex_t *uidRegex, regex_t *dtStampRegexUTC,
                regex_t *dtstartRegexUTC, regex_t *dtendRegexUTC, regex_t *dtStampRegex, regex_t *dtstartRegex,
                regex_t *dtendRegex, regex_t *durationRegex, regex_t *alarmBeginRegex, regex_t *alarmEndRegex);

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
    int valarmCount = 0;
    int alarmCount = 0;
    regex_t beginCalRegex;
    regex_t endCalRegex;
    regex_t beginEveRegex;
    regex_t endEveRegex;
    regex_t versionRegex;
    regex_t proidRegex;
    regex_t uidRegex;

    regex_t dtStampRegexUTC;
    regex_t dtstartRegexUTC;
    regex_t dtendRegexUTC;

    regex_t dtStampRegex;
    regex_t dtstartRegex;
    regex_t dtendRegex;

    regex_t durationRegex;
    regex_t alarmBeginRegex;
    regex_t alarmEndRegex;

    const size_t numMatch = 0;

    char *beginCalPattern = "^[Bb][Ee][Gg][Ii][Nn]:[ ]*[Vv][Cc][Aa][Ll][Ee][Nn][Dd][Aa][Rr][ ]*$";
    char *endCalPattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Cc][Aa][Ll][Ee][Nn][Dd][Aa][Rr][ ]*$";
    char *beginEvePattern = "^[Bb][Ee][Gg][Ii][Nn]:[ ]*[Vv][Ee][Vv][Ee][Nn][Tt][ ]*$";
    char *endEvePattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Ee][Vv][Ee][Nn][Tt][ ]*$";
    char *versionPattern = "^[Vv][Ee][Rr][Ss][Ii][Oo][Nn]:[ ]*[0-9]+\\.[0-9]+[ ]*$";
    char *proidPattern = "^[Pp][Rr][Oo][Dd][Ii][Dd]:[ ]*[0-9A-Za-z/\\\\\\.\\-]{1}.*";
    char *uidPattern = "^[Uu][Ii][Dd]:[ ]*[0-9A-Za-z/\\\\\\.\\-]{1}.*";
    char *dtStampPatternUTC = "[Dd][Tt][Ss][Tt][Aa][Mm][Pp]:[ ]*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[Z][ ]*$";
    char *dtstartPatternUTC = "^[Dd][Tt][Ss][Tt][Aa][Rr][Tt]:[ ]*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[Z]?[ ]*$";
    char *dtendPatternUTC = "^[Dd][Tt][Ee][Nn][Dd]:[ ]*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[Z]?[ ]*$";
    char *dtStampPattern = "[Dd][Tt][Ss][Tt][Aa][Mm][Pp][ ]*;[ ]*[Tt][Zz][Ii][Dd][ ]*=[ ]*.+[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[ ]*$";
    char *dtstartPattern = "^[Dd][Tt][Ss][Tt][Aa][Rr][Tt][ ]*;[ ]*[Tt][Zz][Ii][Dd][ ]*=[ ]*.+[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[ ]*$";
    char *dtendPattern = "^[Dd][Tt][Ee][Nn][Dd][ ]*;[ ]*[Tt][Zz][Ii][Dd][ ]*=[ ]*.+[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[ ]*$";
    char *durationPattern = "^[Dd][Uu][Rr][Aa][Tt][Ii][Oo][Nn]:[ ]*[Pp][Tt][0-9]*[Hh]?[0-9]*[Mm]?[0-9]*[Ss]?[ ]*$";
    char *alarmBeginPattern = "^[Bb][Ee][Gg][Ii][Nn]:[ ]*[Vv][Aa][Ll][Aa][Rr][Mm][ ]*$";
    char *alarmEndPattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Aa][Ll][Aa][Rr][Mm][ ]*$";

    regcomp(&endCalRegex, endCalPattern, REG_NOSUB);
    regcomp(&beginCalRegex, beginCalPattern, REG_NOSUB);
    regcomp(&beginEveRegex, beginEvePattern, REG_NOSUB);
    regcomp(&endEveRegex, endEvePattern, REG_NOSUB);
    regcomp(&versionRegex, versionPattern, REG_EXTENDED);
    regcomp(&proidRegex, proidPattern, REG_EXTENDED);
    regcomp(&uidRegex, uidPattern, REG_EXTENDED);
    regcomp(&dtStampRegexUTC, dtStampPatternUTC, REG_EXTENDED);
    regcomp(&dtstartRegexUTC, dtstartPatternUTC, REG_EXTENDED);
    regcomp(&dtendRegexUTC, dtendPatternUTC, REG_EXTENDED);

    regcomp(&dtStampRegex, dtStampPattern, REG_EXTENDED);
    regcomp(&dtstartRegex, dtstartPattern, REG_EXTENDED);
    regcomp(&dtendRegex, dtendPattern, REG_EXTENDED);


    regcomp(&durationRegex, durationPattern, REG_EXTENDED);
    regcomp(&alarmBeginRegex, alarmBeginPattern, REG_EXTENDED);
    regcomp(&alarmEndRegex, alarmEndPattern, REG_EXTENDED);


    ListIterator iter = createIterator(listOfToken);
    void *elem;
    while ((elem = nextElement(&iter)) != NULL) {
        //VCAL verified.
        if (regexec(&beginCalRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            vcaleCount++;
            calenderCount++;
        }
        if (regexec(&endCalRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            vcaleCount--;
        }
        if (vcaleCount < 0 || vcaleCount > 1) {
            cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                       &uidRegex,
                       &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                       &durationRegex, &alarmBeginRegex, &alarmEndRegex);
            return INV_CAL;
        }
        //vevent verified
        if (regexec(&beginEveRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1) {
                cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                           &uidRegex,
                           &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                           &dtendRegex,
                           &durationRegex, &alarmBeginRegex, &alarmEndRegex);
                return INV_CAL;
            }
            veventCount++;
            eventCount++;
        }
        if (regexec(&endEveRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            veventCount--;
        }
        if (veventCount < 0 || veventCount > 1) {
            cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                       &uidRegex,
                       &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                       &durationRegex, &alarmBeginRegex, &alarmEndRegex);

            return INV_EVENT;
        }


        //valarm verified.
        if (regexec(&alarmBeginRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1 || veventCount != 1) {
                cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                           &uidRegex,
                           &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                           &dtendRegex,
                           &durationRegex, &alarmBeginRegex, &alarmEndRegex);
                return INV_EVENT;
            }
            valarmCount++;
            alarmCount++;
        }
        if (regexec(&alarmEndRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            valarmCount--;
        }
        if (valarmCount < 0 || valarmCount > 1) {
            cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                       &uidRegex,
                       &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                       &durationRegex, &alarmBeginRegex, &alarmEndRegex);
            return INV_EVENT;
        }

        //version verified
        if (regexec(&versionRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1 || veventCount != 0) {
                cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                           &uidRegex,
                           &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                           &dtendRegex,
                           &durationRegex, &alarmBeginRegex, &alarmEndRegex);
                return INV_VER;
            }
            versionCount++;
        }

        //prodid verified
        if (regexec(&proidRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1 || veventCount != 0) {
                cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                           &uidRegex,
                           &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                           &dtendRegex,
                           &durationRegex, &alarmBeginRegex, &alarmEndRegex);
                return INV_PRODID;
            }
            prodidCount++;
        }

        //UID verified
        if (regexec(&uidRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1 || veventCount != 1) {
                cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                           &uidRegex,
                           &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                           &dtendRegex,
                           &durationRegex, &alarmBeginRegex, &alarmEndRegex);
                return INV_EVENT;
            }
            uidCount++;
        }


        //created time verified
        if (regexec(&dtStampRegexUTC, elem, numMatch, NULL, 0) != REG_NOMATCH ||
            regexec(&dtStampRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1 || veventCount != 1) {
                cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                           &uidRegex,
                           &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                           &dtendRegex,
                           &durationRegex, &alarmBeginRegex, &alarmEndRegex);
                return INV_CREATEDT;
            }
            dtStampCount++;
        }

        if (regexec(&dtendRegexUTC, elem, numMatch, NULL, 0) != REG_NOMATCH ||
            regexec(&durationRegex, elem, numMatch, NULL, 0) != REG_NOMATCH ||
            regexec(&dtendRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            dtendCount++;
        }

        //dtstart Verification
        if (regexec(&dtstartRegexUTC, elem, numMatch, NULL, 0) != REG_NOMATCH ||
            regexec(&dtstartRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1 || veventCount != 1) {
                cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                           &uidRegex,
                           &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                           &dtendRegex,
                           &durationRegex, &alarmBeginRegex, &alarmEndRegex);
                return INV_EVENT;
            } else {
                dtstartCount++;
                Node *p = iter.current->previous;
                while (p != NULL && regexec(&endEveRegex, p->data, numMatch, NULL, 0) == REG_NOMATCH) {
                    if (regexec(&alarmBeginRegex, p->data, numMatch, NULL, 0) != REG_NOMATCH) {
                        while (regexec(&alarmEndRegex, p->data, numMatch, NULL, 0) == REG_NOMATCH) {
                            p = p->next;
                            if (p == NULL) {
                                cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex,
                                           &proidRegex, &uidRegex,
                                           &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex,
                                           &dtstartRegex, &dtendRegex,
                                           &durationRegex, &alarmBeginRegex, &alarmEndRegex);
                                return INV_EVENT;
                            }
                        }
                    }


                    if (regexec(&dtendRegexUTC, p->data, numMatch, NULL, 0) != REG_NOMATCH ||
                        regexec(&durationRegex, p->data, numMatch, NULL, 0) != REG_NOMATCH ||
                        regexec(&dtendRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
                        endTimeCount++;
                    }
                    p = p->next;
                }
                if (endTimeCount != 1) {
                    endTimeCount = 0;
                    cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                               &uidRegex,
                               &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                               &dtendRegex,
                               &durationRegex, &alarmBeginRegex, &alarmEndRegex);
                    return INV_EVENT;
                } else {
                    endTimeCount = 0;
                }
            }
        }

    }


    cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
               &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
               &durationRegex, &alarmBeginRegex, &alarmEndRegex);

    if (vcaleCount != 0) {
        return INV_CAL;
    }
    if (veventCount != 0) {
        return INV_EVENT;
    }
    if (versionCount == 0) {
        return INV_VER;
    }
    if (versionCount != calenderCount) {
        return INV_VER;
    }
    if (prodidCount == 0) {
        return INV_PRODID;
    }
    if (versionCount > calenderCount) {
        return DUP_VER;
    }
    if (prodidCount < calenderCount) {
        return INV_PRODID;
    }
    if (prodidCount > calenderCount) {
        return DUP_PRODID;
    }
    if (calenderCount == 0) {
        return INV_CAL;
    }
    if (eventCount == 0) {
        return INV_EVENT;
    }
    if (eventCount < calenderCount) {
        return INV_EVENT;
    }
    if (uidCount != eventCount) {
        return INV_EVENT;
    }
    if (dtStampCount != eventCount) {
        return INV_CREATEDT;
    }
    if ((dtstartCount + alarmCount) != dtendCount) {
        return INV_EVENT;
    }

    return OK;

}

void cleanRegex(regex_t *beginCalRegex, regex_t *endCalRegex, regex_t *beginEveRegex, regex_t *endEveRegex,
                regex_t *versionRegex, regex_t *proidRegex, regex_t *uidRegex, regex_t *dtStampRegexUTC,
                regex_t *dtstartRegexUTC, regex_t *dtendRegexUTC, regex_t *dtStampRegex, regex_t *dtstartRegex,
                regex_t *dtendRegex, regex_t *durationRegex, regex_t *alarmBeginRegex, regex_t *alarmEndRegex) {
    regfree(beginCalRegex);
    regfree(endCalRegex);
    regfree(beginEveRegex);
    regfree(endEveRegex);
    regfree(versionRegex);
    regfree(proidRegex);
    regfree(uidRegex);
    regfree(dtStampRegexUTC);
    regfree(dtstartRegexUTC);
    regfree(dtendRegexUTC);
    regfree(durationRegex);
    regfree(alarmBeginRegex);
    regfree(alarmEndRegex);
    regfree(dtStampRegex);
    regfree(dtstartRegex);
    regfree(dtendRegex);
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

int countCalObject(List *listOfToken) {
    ListIterator iter = createIterator(*listOfToken);
    regex_t beginCalRegex;
    regex_t endCalRegex;
    const size_t numMatch = 0;
    char *beginCalPattern = "^[Bb][Ee][Gg][Ii][Nn]:[ ]*[Vv][Cc][Aa][Ll][Ee][Nn][Dd][Aa][Rr]$";
    char *endCalPattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Cc][Aa][Ll][Ee][Nn][Dd][Aa][Rr]$";
    regcomp(&endCalRegex, endCalPattern, REG_NOSUB);
    regcomp(&beginCalRegex, beginCalPattern, REG_NOSUB);

    int beginCount = 0;
    int endCount = 0;
    void *elem;
    while ((elem = nextElement(&iter)) != NULL) {
        if (regexec(&beginCalRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            beginCount++;
        }
        if (regexec(&endCalRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            endCount++;
        }
    }
    regfree(&beginCalRegex);
    regfree(&endCalRegex);
    if (beginCount == endCount) {
        return beginCount;
    } else {
        return 0;
    }

}


int contentIndicator(void *elem) {
    regex_t beginCalRegex;
    regex_t endCalRegex;
    regex_t beginEveRegex;
    regex_t endEveRegex;
    regex_t versionRegex;
    regex_t proidRegex;
    regex_t uidRegex;

    regex_t dtStampRegexUTC;
    regex_t dtstartRegexUTC;
    regex_t dtendRegexUTC;

    regex_t dtStampRegex;
    regex_t dtstartRegex;
    regex_t dtendRegex;

    regex_t durationRegex;
    regex_t alarmBeginRegex;
    regex_t alarmEndRegex;

    const size_t numMatch = 0;

    char *beginCalPattern = "^[Bb][Ee][Gg][Ii][Nn]:[ ]*[Vv][Cc][Aa][Ll][Ee][Nn][Dd][Aa][Rr]$";
    char *endCalPattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Cc][Aa][Ll][Ee][Nn][Dd][Aa][Rr]$";
    char *beginEvePattern = "^[Bb][Ee][Gg][Ii][Nn]:[ ]*[Vv][Ee][Vv][Ee][Nn][Tt]$";
    char *endEvePattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Ee][Vv][Ee][Nn][Tt]$";
    char *versionPattern = "^[Vv][Ee][Rr][Ss][Ii][Oo][Nn]:[ ]*[0-9]+\\.?[0-9]+$";
    char *proidPattern = "^[Pp][Rr][Oo][Dd][Ii][Dd]:[ ]*[0-9A-Za-z/\\\\\\.\\-]{1}.*";
    char *uidPattern = "^[Uu][Ii][Dd]:[ ]*[0-9A-Za-z/\\\\\\.\\-]{1}.*";

    char *dtStampPatternUTC = "[Dd][Tt][Ss][Tt][Aa][Mm][Pp]:[ ]*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[Z]$";
    char *dtstartPatternUTC = "^[Dd][Tt][Ss][Tt][Aa][Rr][Tt]:[ ]*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[Z]?$";
    char *dtendPatternUTC = "^[Dd][Tt][Ee][Nn][Dd]:[ ]*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[Z]?$";

    char *dtStampPattern = "[Dd][Tt][Ss][Tt][Aa][Mm][Pp][ ]*;[ ]*[Tt][Zz][Ii][Dd][ ]*=[ ]*.+[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}$";
    char *dtstartPattern = "^[Dd][Tt][Ss][Tt][Aa][Rr][Tt][ ]*;[ ]*[Tt][Zz][Ii][Dd][ ]*=[ ]*.+[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}$";
    char *dtendPattern = "^[Dd][Tt][Ee][Nn][Dd][ ]*;[ ]*[Tt][Zz][Ii][Dd][ ]*=[ ]*.+[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}$";


    char *durationPattern = "^[Dd][Uu][Rr][Aa][Tt][Ii][Oo][Nn]:[ ]*[Pp][Tt][0-9]*[Hh]?[0-9]*[Mm]?[0-9]*[Ss]?$";
    char *alarmBeginPattern = "^[Bb][Ee][Gg][Ii][Nn]:[ ]*[Vv][Aa][Ll][Aa][Rr][Mm]$";
    char *alarmEndPattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Aa][Ll][Aa][Rr][Mm]$";

    regcomp(&endCalRegex, endCalPattern, REG_NOSUB);
    regcomp(&beginCalRegex, beginCalPattern, REG_NOSUB);
    regcomp(&beginEveRegex, beginEvePattern, REG_NOSUB);
    regcomp(&endEveRegex, endEvePattern, REG_NOSUB);
    regcomp(&versionRegex, versionPattern, REG_EXTENDED);
    regcomp(&proidRegex, proidPattern, REG_EXTENDED);
    regcomp(&uidRegex, uidPattern, REG_EXTENDED);
    regcomp(&dtStampRegexUTC, dtStampPatternUTC, REG_EXTENDED);
    regcomp(&dtstartRegexUTC, dtstartPatternUTC, REG_EXTENDED);
    regcomp(&dtendRegexUTC, dtendPatternUTC, REG_EXTENDED);

    regcomp(&dtStampRegex, dtStampPattern, REG_EXTENDED);
    regcomp(&dtstartRegex, dtstartPattern, REG_EXTENDED);
    regcomp(&dtendRegex, dtendPattern, REG_EXTENDED);


    regcomp(&durationRegex, durationPattern, REG_EXTENDED);
    regcomp(&alarmBeginRegex, alarmBeginPattern, REG_EXTENDED);
    regcomp(&alarmEndRegex, alarmEndPattern, REG_EXTENDED);


    if (regexec(&beginCalRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
        cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
                   &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                   &durationRegex, &alarmBeginRegex, &alarmEndRegex);
        return 0;
    }
    if (regexec(&endCalRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
        cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
                   &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                   &durationRegex, &alarmBeginRegex, &alarmEndRegex);
        return 1;
    }

    if (regexec(&versionRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
        cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
                   &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                   &durationRegex, &alarmBeginRegex, &alarmEndRegex);
        return 2;
    }

    if (regexec(&proidRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
        cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
                   &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                   &durationRegex, &alarmBeginRegex, &alarmEndRegex);
        return 3;
    }


    cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
               &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
               &durationRegex, &alarmBeginRegex, &alarmEndRegex);
    return -1;
}


char *printEvent(Event *toBePrinted) {
    ListIterator iter = createIterator(toBePrinted->properties);
    char *property = malloc(sizeof(char) * 10);
    strcpy(property, "");
    void *elem;
    while ((elem = nextElement(&iter)) != NULL) {
        char *temp = toBePrinted->properties.printData(elem);
        property = realloc(property, sizeof(char) * (strlen(property) + strlen(temp) + 7));
        strcat(property, temp);
        strcat(property, "\n");
        free(temp);
    }

    ListIterator iter2 = createIterator(toBePrinted->alarms);
    char *alarms = malloc(sizeof(char) * 10);
    strcpy(alarms, "");
    void *elem2;
    while ((elem2 = nextElement(&iter2)) != NULL) {
        char *temp = toBePrinted->alarms.printData(elem2);
        alarms = realloc(alarms, sizeof(char) * (strlen(alarms) + strlen(temp) + 7));
        strcat(alarms, temp);
        strcat(alarms, "\n");
        free(temp);
    }
    int lengthOfCreationDT =
            strlen(toBePrinted->creationDateTime.date) + strlen(toBePrinted->creationDateTime.time) + 30;
    char *creationDateTime = malloc(sizeof(char) * lengthOfCreationDT);
    strcpy(creationDateTime, "Date: ");
    strcat(creationDateTime, toBePrinted->creationDateTime.date);
    strcat(creationDateTime, "\nTime: ");
    strcat(creationDateTime, toBePrinted->creationDateTime.time);
    char *string = malloc(sizeof(char) *
                          (strlen(property) + strlen(alarms) + strlen(creationDateTime) + strlen(toBePrinted->UID) +
                           100));
    strcpy(string, "Event:\nUID: ");
    strcat(string, toBePrinted->UID);
    strcat(string, "\nCreation Date and Time:\n");
    strcat(string, creationDateTime);
    strcat(string, "\nAlarm:\n");
    strcat(string, alarms);
    strcat(string, "\nProperty list: \n");
    strcat(string, property);
    free(property);
    free(alarms);
    free(creationDateTime);
    return string;
}


float getVersionNumber(void *elem) {
    char *num = (char *) elem;
    regex_t versionNumRegex;
    char *versionNumPattern = "[0-9]+\\.?[0-9]+";
    const size_t numMatch = 1;
    regmatch_t strMatch[1];
    regcomp(&versionNumRegex, versionNumPattern, REG_EXTENDED);
    if (regexec(&versionNumRegex, elem, numMatch, strMatch, 0) == REG_NOMATCH) {
        regfree(&versionNumRegex);
        return 0;
    } else {
        char *string = malloc(sizeof(char) * (strMatch[0].rm_eo - strMatch[0].rm_so + 5));
        strncpy(string, (num + strMatch[0].rm_so), (strMatch[0].rm_eo - strMatch[0].rm_so));
        float versionNumber = atof(string);
        free(string);
        return versionNumber;
    }

}

char *getProdid(void *elem) {
    char *prodid = (char *) elem;
    regex_t prodidRegex;
    char *prodidPartten = "[0-9A-Za-z/\\\\\\.\\-]{1}.*";
    const size_t numMatch = 1;
    regmatch_t strMatch[1];
    regcomp(&prodidRegex, prodidPartten, REG_EXTENDED);
    if (regexec(&prodidRegex, elem, numMatch, strMatch, 0) == REG_NOMATCH) {
        regfree(&prodidRegex);
        return NULL;
    } else {
        char *string = malloc(sizeof(char) * (strMatch[0].rm_eo - strMatch[0].rm_so + 5));
        strncpy(string, (prodid + strMatch[0].rm_so), (strMatch[0].rm_eo - strMatch[0].rm_so));
        return string;
    }
}