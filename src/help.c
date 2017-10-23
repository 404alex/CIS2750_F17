#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "help.h"
#include <regex.h>


/**
 * regular expression to free all the compailed regular structure
 * @param beginCalRegex match the pattern: BEGIN:VCALENDAR
 * @param endCalRegex match the pattern: END:VCALENDAR
 * @param beginEveRegex match the pattern: BEGIN:VEVENT
 * @param endEveRegex match the pattern: END:VEVENT
 * @param versionRegex match the pattern: VERSION:....
 * @param proidRegex match the pattern: PRODID:....
 * @param uidRegex match the pattern: UID:....
 * @param dtStampRegexUTC match the pattern: DTSTAMP.... (UTC format)
 * @param dtstartRegexUTC match the pattern: DTSTART...  (UTC format)
 * @param dtendRegexUTC match the pattern: DTEND...  (UTC format)
 * @param dtStampRegex match the pattern: DTSTAMP... (Not UTC format)
 * @param dtstartRegex match the pattern: DTSTART... (Not UTC format)
 * @param dtendRegex match the pattern: DTEND... (Not UTC format)
 * @param durationRegex match the pattern: DURATION...
 * @param alarmBeginRegex match the pattern: BEGIN:ALARM...
 * @param alarmEndRegex match the pattern: END:ALARM
 */
void cleanRegex(regex_t *beginCalRegex, regex_t *endCalRegex, regex_t *beginEveRegex, regex_t *endEveRegex,
                regex_t *versionRegex, regex_t *proidRegex, regex_t *uidRegex, regex_t *dtStampRegexUTC,
                regex_t *dtstartRegexUTC, regex_t *dtendRegexUTC, regex_t *dtStampRegex, regex_t *dtstartRegex,
                regex_t *dtendRegex, regex_t *durationRegex, regex_t *alarmBeginRegex, regex_t *alarmEndRegex);

char *stringToLower(char *string) {
    if (string == NULL) {
        return NULL;
    }
    for (int i = 0; i < strlen(string); ++i) {
        string[i] = tolower(string[i]);
    }
    return string;
}

bool fileNameCheck(char *fileName) {
    if (fileName == NULL) {
        return false;
    }
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
    if (file == NULL) {
        return 0;
    }
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);
    return size;
}


char *readIntoBuffer(FILE *file) {
    if (file == NULL) {
        return NULL;
    }
    char *fileBuffer = malloc(sizeof(char) * (fileSize(file) + 2));
    strcpy(fileBuffer, "");
    char temp[100];
    while ((fgets(temp, 100, file)) != NULL) {
        strcat(fileBuffer, temp);
    }
    return fileBuffer;
}

/**
 * tokenize string by some charaters
 * @param str the string which need to be tokenized
 * @param c the delimit characters
 * @param startPoint the start point of the tokenze, the first time should put 0
 * @return string that after tokenize.
 */
char *tokenize(const char *str, const char c, int *const startPoint) {
    if (str == NULL || startPoint == NULL) {
        return NULL;
    }

    int length = strlen(str);
    if (*startPoint >= length) {
        return "ERROREND";
    }
    char *afterToken = malloc(sizeof(char) * (length + 2 - *startPoint));
    memset(afterToken, '\0', (length + 2 - *startPoint));
    strcpy(afterToken, "");
    int i = 0;
    for (i = *startPoint; i < length + 1; ++i) {
        if (str[i] != c) {
            afterToken[i - *startPoint] = str[i];
        } else {
            i++;
            break;
        }
    }
    afterToken[i - *startPoint] = '\0';
    *startPoint = i + 1;
    return afterToken;
}

void insertTokenizedList(const char *icsFile, List *listOfToken) {
    if (icsFile == NULL) {
        return;
    }
    if (listOfToken == NULL) {
        return;
    }
    int startPoint = 0;
    char *afterToken;
    do {
        afterToken = tokenize(icsFile, '\r', &startPoint);
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
    if (listOfToken == NULL) {
        return false;
    }
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
        if (strncmp("\t", elem, 1) == 0) {
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


ICalErrorCode fileValidation(List listOfToken) {
    int vcaleCount = 0;
    int calenderCount = 0;
    int veventCount = 0;
    int eventCount = 0;
    int versionCount = 0;
    int prodidCount = 0;
    int uidCount = 0;
    int dtStampCount = 0;
    int dtendCount = 0;
    int valarmCount = 0;
    int alarmCount = 0;
    int dtstartCount = 0;
    int alarmActionCount = 0;
    int alarmTriggerCount = 0;
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
    regex_t prodidNoneRegex;
    regex_t versionNoneRegex;
    regex_t noNameRegex;
    regex_t alarmActionRegex;
    regex_t alarmTriggerRegex;
    regex_t allNoNameRegex;
    regex_t ianaRegex;
    regex_t x_compRegex;
    const size_t numMatch = 0;
    char *beginCalPattern = "^[Bb][Ee][Gg][Ii][Nn]:[ ]*[Vv][Cc][Aa][Ll][Ee][Nn][Dd][Aa][Rr][ ]*$";
    char *endCalPattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Cc][Aa][Ll][Ee][Nn][Dd][Aa][Rr][ ]*$";
    char *beginEvePattern = "^[Bb][Ee][Gg][Ii][Nn]:[ ]*[Vv][Ee][Vv][Ee][Nn][Tt][ ]*$";
    char *endEvePattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Ee][Vv][Ee][Nn][Tt][ ]*$";
    char *versionPattern = "^[Vv][Ee][Rr][Ss][Ii][Oo][Nn]:[ ]*[0-9]+\\.?[0-9]*[ ]*$";
    char *proidPattern = "^[Pp][Rr][Oo][Dd][Ii][Dd]:[ ]*[0-9A-Za-z/\\\\\\.\\-]{1}.*";
    char *prodidNonePattern = "^[Pp][Rr][Oo][Dd][Ii][Dd]:[ ]*$";

    char *versionNonePattern = "^[Vv][Ee][Rr][Ss][Ii][Oo][Nn]:[ ]*$";

    char *uidPattern = "^[Uu][Ii][Dd]:[ ]*[0-9A-Za-z/\\\\\\.\\-]{1}.*";
    char *dtStampPatternUTC = "[Dd][Tt][Ss][Tt][Aa][Mm][Pp]:[ ]*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[Z][ ]*$";
    char *dtstartPatternUTC = "^[Dd][Tt][Ss][Tt][Aa][Rr][Tt]:[ ]*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[Z]?[ ]*$";
    char *dtendPatternUTC = "^[Dd][Tt][Ee][Nn][Dd]:[ ]*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[Z]?[ ]*$";

    char *dtStampPattern = "^[Dd][Tt][Ss][Tt][Aa][Mm][Pp][;:][ ]*[Tt]?[Zz]?[Ii]?[Dd]?[ ]*=?[ ]*.*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}$";
    char *dtstartPattern = "^[Dd][Tt][Ss][Tt][Aa][Rr][Tt][;:][ ]*[Tt]?[Zz]?[Ii]?[Dd]?[ ]*=?[ ]*.*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}$";
    char *dtendPattern = "^[Dd][Tt][Ee][Nn][Dd][;:][ ]*[Tt]?[Zz]?[Ii]?[Dd]?[ ]*=?[ ]*.*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}$";

    char *durationPattern = "^[Dd][Uu][Rr][Aa][Tt][Ii][Oo][Nn]:[ ]*[Pp][Tt][0-9]*[Hh]?[0-9]*[Mm]?[0-9]*[Ss]?[ ]*$";
    char *alarmBeginPattern = "^[Bb][Ee][Gg][Ii][Nn]:[ ]*[Vv][Aa][Ll][Aa][Rr][Mm][ ]*$";
    char *alarmEndPattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Aa][Ll][Aa][Rr][Mm][ ]*$";
    char *noNamePattern = "^[A-Za-z]";
    char *alarmActionPattern = "^[Aa][Cc][Tt][Ii][Oo][Nn]:[ ]*[A-Za-z]+";
    char *alarmTriPattern = "^[Tt][Rr][Ii][Gg]{2}[Ee][Rr][:;][ ]*.+$";
    char *allNoNamePattern = "^[A-Za-z\\-]+[:;][ ]*$";
    char *ianaPattern = "^[A-Za-z\\-]+;.*:.*$";
    char *x_compPatter = "^[Xx]\\-";

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
    regcomp(&prodidNoneRegex, prodidNonePattern, REG_EXTENDED);
    regcomp(&versionNoneRegex, versionNonePattern, REG_EXTENDED);
    regcomp(&noNameRegex, noNamePattern, REG_EXTENDED);
    regcomp(&alarmActionRegex, alarmActionPattern, REG_EXTENDED);
    regcomp(&alarmTriggerRegex, alarmTriPattern, REG_EXTENDED);
    regcomp(&allNoNameRegex, allNoNamePattern, REG_EXTENDED);
    regcomp(&ianaRegex, ianaPattern, REG_EXTENDED);
    regcomp(&x_compRegex, x_compPatter, REG_EXTENDED);
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
        if (vcaleCount < 0 || vcaleCount > 1) {    // if find two BEGIN:VCAL without END:VCAL return error.
            cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                       &uidRegex,
                       &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                       &durationRegex, &alarmBeginRegex, &alarmEndRegex);
            regfree(&prodidNoneRegex);
            regfree(&versionNoneRegex);
            regfree(&noNameRegex);
            regfree(&alarmActionRegex);
            regfree(&allNoNameRegex);
            regfree(&alarmTriggerRegex);
            regfree(&ianaRegex);
            regfree(&x_compRegex);
            return INV_CAL;
        }
        //vevent verified
        if (regexec(&beginEveRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1) {     //if find the BEGIN:VEVENT out side the BEGIN:VCAL, return error.
                cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                           &uidRegex,
                           &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                           &dtendRegex,
                           &durationRegex, &alarmBeginRegex, &alarmEndRegex);
                regfree(&prodidNoneRegex);
                regfree(&versionNoneRegex);
                regfree(&noNameRegex);
                regfree(&alarmActionRegex);
                regfree(&allNoNameRegex);
                regfree(&alarmTriggerRegex);
                regfree(&ianaRegex);
                regfree(&x_compRegex);
                return INV_CAL;
            }
            veventCount++;
            eventCount++;
        }
        if (regexec(&endEveRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            veventCount--;
        }
        if (veventCount < 0 || veventCount > 1) {    //if find two BEGIN:VEVENT without END:VEVENT return error.
            cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                       &uidRegex,
                       &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                       &durationRegex, &alarmBeginRegex, &alarmEndRegex);
            regfree(&prodidNoneRegex);
            regfree(&versionNoneRegex);
            regfree(&noNameRegex);
            regfree(&alarmActionRegex);
            regfree(&allNoNameRegex);
            regfree(&alarmTriggerRegex);
            regfree(&ianaRegex);
            regfree(&x_compRegex);
            return INV_EVENT;
        }


        //valarm verified.
        if (regexec(&alarmBeginRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1 || veventCount != 1) {   // if find BEGIN:VALARM out side the VEVENT, return error.
                cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                           &uidRegex,
                           &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                           &dtendRegex,
                           &durationRegex, &alarmBeginRegex, &alarmEndRegex);
                regfree(&prodidNoneRegex);
                regfree(&versionNoneRegex);
                regfree(&noNameRegex);
                regfree(&alarmActionRegex);
                regfree(&allNoNameRegex);
                regfree(&alarmTriggerRegex);
                regfree(&ianaRegex);
                regfree(&x_compRegex);
                return INV_EVENT;
            }
            valarmCount++;
            alarmCount++;
        }
        if (regexec(&alarmEndRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            valarmCount--;
        }
        if (valarmCount < 0 || valarmCount > 1) {   // if find two BEGIN:VALARM, without one END:VALARM return error.
            cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                       &uidRegex,
                       &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                       &durationRegex, &alarmBeginRegex, &alarmEndRegex);
            regfree(&prodidNoneRegex);
            regfree(&versionNoneRegex);
            regfree(&noNameRegex);
            regfree(&alarmActionRegex);
            regfree(&allNoNameRegex);
            regfree(&alarmTriggerRegex);
            regfree(&ianaRegex);
            regfree(&x_compRegex);
            return INV_EVENT;
        }

        //version verified
        if (regexec(&versionRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1 || veventCount !=
                                   0) {  //if find version outside BEGIN:VCAL or inside BEGIN:VENEVT, BEGIN:VALARM return error.
                cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                           &uidRegex,
                           &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                           &dtendRegex,
                           &durationRegex, &alarmBeginRegex, &alarmEndRegex);
                regfree(&prodidNoneRegex);
                regfree(&versionNoneRegex);
                regfree(&noNameRegex);
                regfree(&alarmActionRegex);
                regfree(&allNoNameRegex);
                regfree(&alarmTriggerRegex);
                regfree(&ianaRegex);
                regfree(&x_compRegex);
                return INV_VER;
            }
            versionCount++;
        }

        if (regexec(&prodidNoneRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                       &uidRegex,
                       &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                       &dtendRegex,
                       &durationRegex, &alarmBeginRegex, &alarmEndRegex);
            regfree(&prodidNoneRegex);
            regfree(&versionNoneRegex);
            regfree(&noNameRegex);
            regfree(&alarmActionRegex);
            regfree(&allNoNameRegex);
            regfree(&alarmTriggerRegex);
            regfree(&ianaRegex);
            regfree(&x_compRegex);
            return INV_PRODID;
        }

        //prodid verified
        if (regexec(&proidRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1 || veventCount !=
                                   0) {  //if find prodid outside BEGIN:VCAL or inside BEGIN:VENEVT, BEGIN:VALARM return error.
                cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                           &uidRegex,
                           &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                           &dtendRegex,
                           &durationRegex, &alarmBeginRegex, &alarmEndRegex);
                regfree(&prodidNoneRegex);
                regfree(&versionNoneRegex);
                regfree(&noNameRegex);
                regfree(&alarmActionRegex);
                regfree(&allNoNameRegex);
                regfree(&alarmTriggerRegex);
                regfree(&ianaRegex);
                regfree(&x_compRegex);
                return INV_PRODID;
            }
            prodidCount++;
        }

        //UID verified
        if (regexec(&uidRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1 || veventCount != 1) {  //if find the UID not inside the BEGIN:VEVENT, return error.
                cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                           &uidRegex,
                           &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                           &dtendRegex,
                           &durationRegex, &alarmBeginRegex, &alarmEndRegex);
                regfree(&prodidNoneRegex);
                regfree(&versionNoneRegex);
                regfree(&noNameRegex);
                regfree(&alarmActionRegex);
                regfree(&allNoNameRegex);
                regfree(&alarmTriggerRegex);
                regfree(&ianaRegex);
                regfree(&x_compRegex);
                return INV_EVENT;
            }
            uidCount++;
        }

        if (regexec(&alarmTriggerRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            alarmTriggerCount++;
        }


        //created time verified
        if (regexec(&dtStampRegexUTC, elem, numMatch, NULL, 0) != REG_NOMATCH ||
            regexec(&dtStampRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            if (vcaleCount != 1 || veventCount != 1) {  //if find the DTStamp not in the right place return error.
                cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                           &uidRegex,
                           &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                           &dtendRegex,
                           &durationRegex, &alarmBeginRegex, &alarmEndRegex);
                regfree(&prodidNoneRegex);
                regfree(&versionNoneRegex);
                regfree(&noNameRegex);
                regfree(&alarmActionRegex);
                regfree(&allNoNameRegex);
                regfree(&alarmTriggerRegex);
                regfree(&ianaRegex);
                regfree(&x_compRegex);
                return INV_CREATEDT;
            }
            dtStampCount++;
        }

        if (regexec(&durationRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            if (valarmCount == 0) {
                dtendCount++;
            }
        }
        if (regexec(&dtendRegexUTC, elem, numMatch, NULL, 0) != REG_NOMATCH ||
            regexec(&dtendRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            dtendCount++;
        }
        if (regexec(&versionNoneRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                       &uidRegex,
                       &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                       &dtendRegex,
                       &durationRegex, &alarmBeginRegex, &alarmEndRegex);
            regfree(&prodidNoneRegex);
            regfree(&versionNoneRegex);
            regfree(&noNameRegex);
            regfree(&alarmActionRegex);
            regfree(&allNoNameRegex);
            regfree(&alarmTriggerRegex);
            regfree(&ianaRegex);
            regfree(&x_compRegex);
            return INV_VER;
        }
        if (regexec(&noNameRegex, elem, numMatch, NULL, 0) == REG_NOMATCH) {
            cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                       &uidRegex,
                       &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                       &dtendRegex,
                       &durationRegex, &alarmBeginRegex, &alarmEndRegex);
            regfree(&prodidNoneRegex);
            regfree(&versionNoneRegex);
            regfree(&noNameRegex);
            regfree(&alarmActionRegex);
            regfree(&allNoNameRegex);
            regfree(&alarmTriggerRegex);
            regfree(&ianaRegex);
            regfree(&x_compRegex);
            if (valarmCount == 1) {
                return INV_ALARM;
            }
            if (veventCount == 1) {
                return INV_EVENT;
            }
            if (vcaleCount == 1) {
                return INV_CAL;
            } else {
                return INV_CAL;
            }
        }


        if (regexec(&allNoNameRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
                       &uidRegex,
                       &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
                       &dtendRegex,
                       &durationRegex, &alarmBeginRegex, &alarmEndRegex);
            regfree(&prodidNoneRegex);
            regfree(&versionNoneRegex);
            regfree(&noNameRegex);
            regfree(&alarmActionRegex);
            regfree(&allNoNameRegex);
            regfree(&alarmTriggerRegex);
            regfree(&ianaRegex);
            regfree(&x_compRegex);
            if (valarmCount == 1) {
                return INV_ALARM;
            }
            if (veventCount == 1) {
                return INV_EVENT;
            }
            if (vcaleCount == 1) {
                return INV_CAL;
            } else {
                return INV_CAL;
            }
        }


//        if (regexec(&ianaRegex, elem, numMatch, NULL, 0) != REG_NOMATCH ||
//            regexec(&x_compRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
//            cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex,
//                       &uidRegex,
//                       &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex,
//                       &dtendRegex,
//                       &durationRegex, &alarmBeginRegex, &alarmEndRegex);
//            regfree(&prodidNoneRegex);
//            regfree(&versionNoneRegex);
//            regfree(&noNameRegex);
//            regfree(&alarmActionRegex);
//            regfree(&allNoNameRegex);
//            regfree(&alarmTriggerRegex);
//            regfree(&ianaRegex);
//            regfree(&x_compRegex);
//            if (valarmCount == 1) {
//                return INV_ALARM;
//            }
//            if (veventCount == 1) {
//                return INV_EVENT;
//            }
//            if (vcaleCount == 1) {
//                return INV_CAL;
//            } else {
//                return INV_CAL;
//            }
//        }


        if (regexec(&dtstartRegex, elem, numMatch, NULL, 0) != REG_NOMATCH ||
            regexec(&dtstartRegexUTC, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            dtstartCount++;
        }
        if (regexec(&alarmActionRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
            alarmActionCount++;
        }
    }

    regfree(&alarmActionRegex);
    regfree(&noNameRegex);
    regfree(&versionNoneRegex);
    regfree(&prodidNoneRegex);
    regfree(&alarmActionRegex);
    regfree(&allNoNameRegex);
    regfree(&alarmTriggerRegex);
    regfree(&ianaRegex);
    regfree(&x_compRegex);
    cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
               &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
               &durationRegex, &alarmBeginRegex, &alarmEndRegex);

    if (vcaleCount != 0) {
        //if there is some BEGIN:VCAL do not have valid END:VCAL return error.
        return INV_CAL;
    }
    if (veventCount != 0) {
        // if there is some BEGIN:VEVENT do not have valid END:VEVENT, return error.
        return INV_EVENT;
    }
    if (versionCount == 0) {
        // if in the file, there is no versionNumver, return error.
        return INV_CAL;
    }
    if (versionCount < calenderCount) {
        //if in the file, there is some BEGIN:VCAL pairs do not have a version number. return error.
        return INV_CAL;
    }
    if (versionCount > calenderCount) {
        //if in the file, the number of version values more than the number of the BEGIN:VCAL pair, return error.
        return DUP_VER;
    }
    if (prodidCount == 0) {
        //if in the file, there is no prodid, return error.
        return INV_CAL;
    }
    if (prodidCount < calenderCount) {
        //if in the file, there is some BEGIN:VCAL pairs do not have a prodid. return error.
        return INV_PRODID;
    }
    if (prodidCount > calenderCount) {
        //if in the file, the number of prodid values more than the number of the BEGIN:VCAL pair, return error.
        return DUP_PRODID;
    }
    if (calenderCount == 0) {
        //if no canlendar component in the file, return error.
        return INV_CAL;
    }
    if (eventCount == 0) {
        // if no event component in the file, return error.
        return INV_EVENT;
    }
    if (eventCount < calenderCount) {
        // if the event components less than the calendar components, return error.
        return INV_EVENT;
    }
    if (uidCount != eventCount) {
        // if the uid properites number not equal to the event component number, return error.
        return INV_EVENT;
    }
    if (dtStampCount != eventCount) {
        // if the dtstamp properties number not equal to the event component number, return error.
        return INV_CREATEDT;
    }
    if (dtstartCount != dtendCount) {
        return INV_EVENT;
    }
    if (alarmActionCount != alarmCount) {
        return INV_ALARM;
    }
    if (alarmTriggerCount != alarmCount) {
        return INV_ALARM;
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
    if (listOfToken == NULL) {
        return;
    }
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
    if (listOfToken == NULL) {
        return -1;
    }
    if (listOfToken->length == 0) {
        return -1;
    }
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
    if (elem == NULL) {
        return -100;
    }
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
    regex_t alarmActionRegex;
    regex_t durationRegex;
    regex_t alarmBeginRegex;
    regex_t alarmEndRegex;
    regex_t alarmTriRegex;
    const size_t numMatch = 0;
    char *beginCalPattern = "^[Bb][Ee][Gg][Ii][Nn]:[ ]*[Vv][Cc][Aa][Ll][Ee][Nn][Dd][Aa][Rr]$";
    char *endCalPattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Cc][Aa][Ll][Ee][Nn][Dd][Aa][Rr]$";
    char *beginEvePattern = "^[Bb][Ee][Gg][Ii][Nn]:[ ]*[Vv][Ee][Vv][Ee][Nn][Tt]$";
    char *endEvePattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Ee][Vv][Ee][Nn][Tt]$";
    char *versionPattern = "^[Vv][Ee][Rr][Ss][Ii][Oo][Nn]:[ ]*[0-9]+\\.?[0-9]*$";
    char *proidPattern = "^[Pp][Rr][Oo][Dd][Ii][Dd]:[ ]*[0-9A-Za-z/\\\\\\.\\-]{1}.*";
    char *uidPattern = "^[Uu][Ii][Dd]:[ ]*[0-9A-Za-z/\\\\\\.\\-]{1}.*";
    char *dtStampPatternUTC = "[Dd][Tt][Ss][Tt][Aa][Mm][Pp]:[ ]*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[Z]$";
    char *dtstartPatternUTC = "^[Dd][Tt][Ss][Tt][Aa][Rr][Tt]:[ ]*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[Z]?$";
    char *dtendPatternUTC = "^[Dd][Tt][Ee][Nn][Dd]:[ ]*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}[Z]?$";
    char *dtStampPattern = "^[Dd][Tt][Ss][Tt][Aa][Mm][Pp][;:][ ]*[Tt]?[Zz]?[Ii]?[Dd]?[ ]*=?[ ]*.*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}$";
    char *dtstartPattern = "^[Dd][Tt][Ss][Tt][Aa][Rr][Tt][;:][ ]*[Tt]?[Zz]?[Ii]?[Dd]?[ ]*=?[ ]*.*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}$";
    char *dtendPattern = "^[Dd][Tt][Ee][Nn][Dd][;:][ ]*[Tt]?[Zz]?[Ii]?[Dd]?[ ]*=?[ ]*.*[1-9][0-9]{7}[A-Za-z]{1}[0-9]{6}$";
    char *durationPattern = "^[Dd][Uu][Rr][Aa][Tt][Ii][Oo][Nn]:[ ]*[Pp][Tt][0-9]*[Hh]?[0-9]*[Mm]?[0-9]*[Ss]?$";
    char *alarmBeginPattern = "^[Bb][Ee][Gg][Ii][Nn]:[ ]*[Vv][Aa][Ll][Aa][Rr][Mm]$";
    char *alarmEndPattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Aa][Ll][Aa][Rr][Mm]$";
    char *alarmActionPattern = "^[Aa][Cc][Tt][Ii][Oo][Nn]";
    char *alarmTriPattern = "^[Tt][Rr][Ii][Gg]{2}[Ee][Rr]";
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
    regcomp(&alarmActionRegex, alarmActionPattern, REG_EXTENDED);
    regcomp(&alarmTriRegex, alarmTriPattern, REG_EXTENDED);
    if (regexec(&beginCalRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
        cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
                   &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                   &durationRegex, &alarmBeginRegex, &alarmEndRegex);
        regfree(&alarmActionRegex);
        regfree(&alarmTriRegex);
        return 0;
    }
    if (regexec(&endCalRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
        cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
                   &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                   &durationRegex, &alarmBeginRegex, &alarmEndRegex);
        regfree(&alarmActionRegex);
        regfree(&alarmTriRegex);

        return 1;
    }

    if (regexec(&versionRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
        cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
                   &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                   &durationRegex, &alarmBeginRegex, &alarmEndRegex);
        regfree(&alarmActionRegex);
        regfree(&alarmTriRegex);
        return 2;
    }

    if (regexec(&proidRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
        cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
                   &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                   &durationRegex, &alarmBeginRegex, &alarmEndRegex);
        regfree(&alarmActionRegex);
        regfree(&alarmTriRegex);
        return 3;
    }
    if (regexec(&beginEveRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
        cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
                   &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                   &durationRegex, &alarmBeginRegex, &alarmEndRegex);
        regfree(&alarmActionRegex);
        regfree(&alarmTriRegex);
        return 4;
    }

    if (regexec(&endEveRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
        cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
                   &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                   &durationRegex, &alarmBeginRegex, &alarmEndRegex);
        regfree(&alarmActionRegex);
        regfree(&alarmTriRegex);
        return 5;
    }
    if (regexec(&uidRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
        cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
                   &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                   &durationRegex, &alarmBeginRegex, &alarmEndRegex);
        regfree(&alarmActionRegex);
        regfree(&alarmTriRegex);
        return 6;
    }
    if (regexec(&dtStampRegexUTC, elem, numMatch, NULL, 0) != REG_NOMATCH) {
        cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
                   &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                   &durationRegex, &alarmBeginRegex, &alarmEndRegex);
        regfree(&alarmActionRegex);
        regfree(&alarmTriRegex);
        return 7;
    }
    if (regexec(&dtStampRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
        cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
                   &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                   &durationRegex, &alarmBeginRegex, &alarmEndRegex);
        regfree(&alarmActionRegex);
        regfree(&alarmTriRegex);
        return 8;
    }

    if (regexec(&alarmBeginRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
        cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
                   &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                   &durationRegex, &alarmBeginRegex, &alarmEndRegex);
        regfree(&alarmActionRegex);
        regfree(&alarmTriRegex);
        return 9;
    }

    if (regexec(&alarmActionRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
        cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
                   &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                   &durationRegex, &alarmBeginRegex, &alarmEndRegex);
        regfree(&alarmActionRegex);
        regfree(&alarmTriRegex);
        return 10;
    }
    if (regexec(&alarmTriRegex, elem, numMatch, NULL, 0) != REG_NOMATCH) {
        cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
                   &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
                   &durationRegex, &alarmBeginRegex, &alarmEndRegex);
        regfree(&alarmActionRegex);
        regfree(&alarmTriRegex);
        return 11;
    }


    cleanRegex(&beginCalRegex, &endCalRegex, &beginEveRegex, &endEveRegex, &versionRegex, &proidRegex, &uidRegex,
               &dtStampRegexUTC, &dtstartRegexUTC, &dtendRegexUTC, &dtStampRegex, &dtstartRegex, &dtendRegex,
               &durationRegex, &alarmBeginRegex, &alarmEndRegex);
    regfree(&alarmActionRegex);
    regfree(&alarmTriRegex);
    return -1;
}


char *printEvent(void *toBePrinted) {
    Event *event = (Event *) toBePrinted;
    if (event == NULL) {
        return NULL;
    }
    ListIterator iter = createIterator(event->properties);
    char *property = malloc(sizeof(char) * 10);
    strcpy(property, "");
    void *elem;
    while ((elem = nextElement(&iter)) != NULL) {
        char *temp = event->properties.printData(elem);
        property = realloc(property, sizeof(char) * (strlen(property) + strlen(temp) + 7));
        strcat(property, temp);
        strcat(property, "\r\n");
        free(temp);
    }

    ListIterator iter2 = createIterator(event->alarms);
    char *alarms = malloc(sizeof(char) * 10);
    strcpy(alarms, "");
    void *elem2;
    while ((elem2 = nextElement(&iter2)) != NULL) {
        char *temp = event->alarms.printData(elem2);
        alarms = realloc(alarms, sizeof(char) * (strlen(alarms) + strlen(temp) + 7));
        strcat(alarms, temp);
        strcat(alarms, "\r\n");
        free(temp);
    }
    int lengthOfCreationDT =
            strlen(event->creationDateTime.date) + strlen(event->creationDateTime.time) + 30;
    char *creationDateTime = malloc(sizeof(char) * lengthOfCreationDT);
    strcpy(creationDateTime, event->creationDateTime.date);
    strcat(creationDateTime, ":");
    strcat(creationDateTime, event->creationDateTime.time);
    if (event->creationDateTime.UTC == true) {
        strcat(creationDateTime, ", UTC=1");
    } else {
        strcat(creationDateTime, ", UTC=0");
    }
    char *string = malloc(sizeof(char) *
                          (strlen(property) + strlen(alarms) + strlen(creationDateTime) + strlen(event->UID) +
                           100));
    strcpy(string, "\tUID = ");
    strcat(string, event->UID);
    strcat(string, "\n\tcreationDateTime = ");
    strcat(string, creationDateTime);
    strcat(string, "\n\tAlarms:\n");
    strcat(string, alarms);
    strcat(string, "\tOther properties: \n");
    strcat(string, property);
    free(property);
    free(alarms);
    free(creationDateTime);
    return string;
}

char *writeProperty(void *toBePrinted) {
    if (toBePrinted == NULL) {
        return NULL;
    }
    Property *aProperty = (Property *) toBePrinted;
    int length = strlen(aProperty->propName) + strlen(aProperty->propDescr) + 5;
    char *string = malloc(sizeof(char) * length);
    strcpy(string, "");
    strcat(string, aProperty->propName);
    strcat(string, ":");
    strcat(string, aProperty->propDescr);
    return string;
}


char *writeEvent(void *toBePrinted) {
    Event *event = (Event *) toBePrinted;
    if (event == NULL) {
        return NULL;
    }
    ListIterator iter = createIterator(event->properties);
    char *property = malloc(sizeof(char) * 20);
    strcpy(property, "");
    void *elem;
    while ((elem = nextElement(&iter)) != NULL) {
        char *temp = writeProperty(elem);
        property = realloc(property, sizeof(char) * (strlen(property) + strlen(temp) + 7));
        strcat(property, temp);
        strcat(property, "\r\n");
        free(temp);
    }

    ListIterator iter2 = createIterator(event->alarms);
    char *alarms = malloc(sizeof(char) * 20);
    strcpy(alarms, "");
    void *elem2;
    while ((elem2 = nextElement(&iter2)) != NULL) {
        char *temp = writeAlarm(elem2);
        alarms = realloc(alarms, sizeof(char) * (strlen(alarms) + strlen(temp) + 7));
        strcat(alarms, temp);
        strcat(alarms, "\r\n");
        free(temp);
    }
    int lengthOfCreationDT =
            strlen(event->creationDateTime.date) + strlen(event->creationDateTime.time) + 30;
    char *creationDateTime = malloc(sizeof(char) * lengthOfCreationDT);
    strcpy(creationDateTime, event->creationDateTime.date);
    strcat(creationDateTime, "T");
    strcat(creationDateTime, event->creationDateTime.time);
    if (event->creationDateTime.UTC == true) {
        strcat(creationDateTime, "Z");
    }
    char *string = malloc(sizeof(char) *
                          (strlen(property) + strlen(alarms) + strlen(creationDateTime) + strlen(event->UID) +
                           300));
    strcpy(string, "BEGIN:VEVENT\r\n");
    strcat(string, "UID:");
    strcat(string, event->UID);
    strcat(string, "\r\nDTSTAMP:");
    strcat(string, creationDateTime);
    strcat(string, "\r\n");
    strcat(string, property);
    strcat(string, alarms);
    strcat(string, "END:VEVENT");
    free(property);
    free(alarms);
    free(creationDateTime);
    return string;
}


float getVersionNumber(void *elem) {
    if (elem == NULL) {
        return 0;
    }
    char *num = (char *) elem;
    regex_t versionNumRegex;
    char *versionNumPattern = "[0-9]+\\.?[0-9]*";
    const size_t numMatch = 1;
    regmatch_t strMatch[1];
    regcomp(&versionNumRegex, versionNumPattern, REG_EXTENDED);
    if (regexec(&versionNumRegex, elem, numMatch, strMatch, 0) == REG_NOMATCH) {
        regfree(&versionNumRegex);
        return 0;
    } else {
        char *string = malloc(sizeof(char) * (strMatch[0].rm_eo - strMatch[0].rm_so + 5));
        sprintf(string, "%.*s", (int) (strMatch[0].rm_eo - strMatch[0].rm_so), (num + strMatch[0].rm_so));
        float versionNumber = atof(string);
        free(string);
        regfree(&versionNumRegex);
        return versionNumber;
    }

}

char *getProdid(void *elem) {
    if (elem == NULL) {
        return NULL;
    }
    char *prodid = (char *) elem;
    regex_t prodidRegex;
    char *prodidPartten = "^[Pp][Rr][Oo][Dd][Ii][Dd]:[ ]*";
    const size_t numMatch = 1;
    regmatch_t strMatch[1];
    regcomp(&prodidRegex, prodidPartten, REG_EXTENDED);
    if (regexec(&prodidRegex, elem, numMatch, strMatch, 0) == REG_NOMATCH) {
        regfree(&prodidRegex);
        return NULL;
    } else {
        char *string = malloc(sizeof(char) * (strlen(prodid) - strMatch[0].rm_eo + 5));
        sprintf(string, "%.*s", (int) (strlen(prodid) - strMatch[0].rm_eo), (prodid + strMatch[0].rm_eo));
        regfree(&prodidRegex);
        return string;
    }
}

bool isEndEvent(void *elem) {
    if (elem == NULL) {
        return false;
    }
    char *string = (char *) elem;
    regex_t endeventRegex;
    const size_t numMatch = 0;
    char *endEvePattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Ee][Vv][Ee][Nn][Tt]$";
    regcomp(&endeventRegex, endEvePattern, REG_EXTENDED);
    if (regexec(&endeventRegex, string, numMatch, NULL, 0) != REG_NOMATCH) {
        regfree(&endeventRegex);
        return true;
    } else {
        regfree(&endeventRegex);
        return false;
    }
}

char *getUID(void *elem) {
    if (elem == NULL) {
        return NULL;
    }
    char *UID = (char *) elem;
    regex_t uidRegex;
    char *uidPattern = "[0-9A-Za-z/\\\\\\.\\-]{1}.*";
    const size_t numMatch = 1;
    regmatch_t strMatch[1];
    regcomp(&uidRegex, uidPattern, REG_EXTENDED);
    if (regexec(&uidRegex, elem + 3, numMatch, strMatch, 0) == REG_NOMATCH) {
        regfree(&uidRegex);
        return NULL;
    } else {
        char *string = malloc(sizeof(char) * (strMatch[0].rm_eo - strMatch[0].rm_so + 5));
        sprintf(string, "%.*s", (strMatch[0].rm_eo - strMatch[0].rm_so), (UID + strMatch[0].rm_so + 3));
        regfree(&uidRegex);
        return string;
    }
}


char *getUTCDate(void *elem) {
    if (elem == NULL) {
        return NULL;
    }
    char *date = (char *) elem;
    regex_t dateRegex;
    char *datePattern = "[0-9]{8}";
    const size_t numMatch = 1;
    regmatch_t strMatch[1];
    regcomp(&dateRegex, datePattern, REG_EXTENDED);
    if (regexec(&dateRegex, elem, numMatch, strMatch, 0) == REG_NOMATCH) {
        regfree(&dateRegex);
        return NULL;
    } else {
        char *string = malloc(sizeof(char) * (strMatch[0].rm_eo - strMatch[0].rm_so + 5));
        sprintf(string, "%.*s", (strMatch[0].rm_eo - strMatch[0].rm_so), (date + strMatch[0].rm_so));
        regfree(&dateRegex);
        return string;
    }
}

char *getUTCTime(void *elem) {
    if (elem == NULL) {
        return NULL;
    }
    char *date = (char *) elem;
    regex_t dateRegex;
    char *datePattern = "[0-9]{6}[Z]$";
    const size_t numMatch = 1;
    regmatch_t strMatch[1];
    regcomp(&dateRegex, datePattern, REG_EXTENDED);
    if (regexec(&dateRegex, elem, numMatch, strMatch, 0) == REG_NOMATCH) {
        regfree(&dateRegex);
        return NULL;
    } else {
        char *string = malloc(sizeof(char) * (strMatch[0].rm_eo - strMatch[0].rm_so + 5));
        sprintf(string, "%.*s", (strMatch[0].rm_eo - strMatch[0].rm_so - 1), (date + strMatch[0].rm_so));
        regfree(&dateRegex);
        return string;
    }
}

char *getTime(void *elem) {
    if (elem == NULL) {
        return NULL;
    }
    char *date = (char *) elem;
    regex_t dateRegex;
    char *datePattern = "[0-9]{6}$";
    const size_t numMatch = 1;
    regmatch_t strMatch[1];
    regcomp(&dateRegex, datePattern, REG_EXTENDED);
    if (regexec(&dateRegex, elem, numMatch, strMatch, 0) == REG_NOMATCH) {
        regfree(&dateRegex);
        return NULL;
    } else {
        char *string = malloc(sizeof(char) * (strMatch[0].rm_eo - strMatch[0].rm_so + 5));
        sprintf(string, "%.*s", (strMatch[0].rm_eo - strMatch[0].rm_so), (date + strMatch[0].rm_so));
        regfree(&dateRegex);
        return string;
    }
}

bool isEndAlarm(void *elem) {
    if (elem == NULL) {
        return false;
    }
    char *alarm = (char *) elem;
    char *alarmEndPattern = "^[Ee][Nn][Dd]:[ ]*[Vv][Aa][Ll][Aa][Rr][Mm]$";
    regex_t alarmRegex;
    const size_t numMatch = 0;
    regcomp(&alarmRegex, alarmEndPattern, REG_EXTENDED);
    if (regexec(&alarmRegex, alarm, numMatch, NULL, 0) != REG_EXTENDED) {
        regfree(&alarmRegex);
        return true;
    } else {
        regfree(&alarmRegex);
        return false;
    }
}

char *getAlarmAction(void *elem) {
    if (elem == NULL) {
        return NULL;
    }
    char *action = (char *) elem;
    regex_t actionRegex;
    char *actionPattern = "^[Aa][Cc][Tt][Ii][Oo][Nn]:[ ]*";
    const size_t numMatch = 1;
    regmatch_t strMatch[1];
    regcomp(&actionRegex, actionPattern, REG_EXTENDED);
    if (regexec(&actionRegex, action, numMatch, strMatch, 0) == REG_NOMATCH) {
        regfree(&actionRegex);
        return NULL;
    } else {
        char *string = malloc(sizeof(char) * (strlen(action) - strMatch[0].rm_eo + 5));
        sprintf(string, "%.*s", (int) (strlen(action) - strMatch[0].rm_eo), (action + strMatch[0].rm_eo));
        regfree(&actionRegex);
        return string;
    }
}

char *getAlarmTri(void *elem) {
    if (elem == NULL) {
        return NULL;
    }
    char *trigger = (char *) elem;
    regex_t triggerRegex;
    char *triggerPattern = "^[Tt][Rr][Ii][Gg]{2}[Ee][Rr][:;]{1}";
    const size_t numMatch = 1;
    regmatch_t strMatch[1];
    regcomp(&triggerRegex, triggerPattern, REG_EXTENDED);
    if (regexec(&triggerRegex, trigger, numMatch, strMatch, 0) == REG_NOMATCH) {
        regfree(&triggerRegex);
        return NULL;
    } else {
        char *string = malloc(sizeof(char) * (strlen(trigger) - strMatch[0].rm_eo + 5));
        sprintf(string, "%.*s", (int) (strlen(trigger) - strMatch[0].rm_eo), (trigger + strMatch[0].rm_eo));
        regfree(&triggerRegex);
        return string;
    }
}

char *getName(void *elem) {
    if (elem == NULL) {
        return NULL;
    }
    char *name = (char *) elem;
    regex_t nameRegex;
    char *namePattern = "^[A-Za-z\\-]+[:;]{1}";
    const size_t numMatch = 1;
    regmatch_t strMatch[1];
    regcomp(&nameRegex, namePattern, REG_EXTENDED);
    if (regexec(&nameRegex, name, numMatch, strMatch, 0) == REG_NOMATCH) {
        regfree(&nameRegex);
        return NULL;
    } else {
        char *string = malloc(sizeof(char) * (strMatch[0].rm_eo - strMatch[0].rm_so + 5));
        sprintf(string, "%.*s", (strMatch[0].rm_eo - strMatch[0].rm_so - 1), (name + strMatch[0].rm_so));
        regfree(&nameRegex);
        return string;
    }
}

char *getDescription(void *elem) {
    if (elem == NULL) {
        return NULL;
    }
    char *des = (char *) elem;
    regex_t desRegex;
    char *desPattern = "^[A-Za-z\\-]+[:;][ ]*";
    const size_t numMatch = 1;
    regmatch_t strMatch[1];
    regcomp(&desRegex, desPattern, REG_EXTENDED);
    if (regexec(&desRegex, des, numMatch, strMatch, 0) == REG_NOMATCH) {
        regfree(&desRegex);
        return NULL;
    } else {
        char *string = malloc(sizeof(char) * (strlen(des) - strMatch[0].rm_so + 5));
        sprintf(string, "%.*s", (int) (strlen(des) - strMatch[0].rm_eo), (des + strMatch[0].rm_eo));
        regfree(&desRegex);
        return string;
    }
}


char *printToken(void *toBePrinted) {
    if (toBePrinted == NULL) {
        return NULL;
    }
    char *string = malloc(sizeof(char) * (strlen((char *) toBePrinted) + 5));
    strcpy(string, (char *) toBePrinted);
    return string;
}

int compartToken(const void *first, const void *second) {
    if (first == NULL || second == NULL) {
        return 0;
    }
    return strcmp(first, second);
}

void deleteToken(void *toBeDeleted) {
    if (toBeDeleted == NULL) {
        return;
    }
    free(toBeDeleted);
}

char *printAlarm(void *toBePrinted) {
    if (toBePrinted == NULL) {
        return NULL;
    }
    Alarm *alarm = (Alarm *) toBePrinted;
    ListIterator iter = createIterator(alarm->properties);
    char *property = malloc(sizeof(char) * 10);
    strcpy(property, "");
    void *elem;
    while ((elem = nextElement(&iter)) != NULL) {
        char *temp = alarm->properties.printData(elem);
        property = realloc(property, sizeof(char) * (strlen(property) + strlen(temp) + 7));
        strcat(property, temp);
        strcat(property, "\r\n");
        free(temp);
    }
    char *string = malloc(sizeof(char) * (strlen(property) + strlen(alarm->action) + strlen(alarm->trigger) + 50));
    strcpy(string, "\t\tAction: ");
    strcat(string, alarm->action);
    strcat(string, "\r\n\t\tTrigger: ");
    strcat(string, alarm->trigger);
    strcat(string, "\r\n\t\tProperties:\r\n");
    strcat(string, property);
    free(property);
    return string;
}


char *writeAlarm(void *toBePrinted) {
    if (toBePrinted == NULL) {
        return NULL;
    }
    Alarm *alarm = (Alarm *) toBePrinted;
    ListIterator iter = createIterator(alarm->properties);
    char *property = malloc(sizeof(char) * 50);
    strcpy(property, "");
    void *elem;
    while ((elem = nextElement(&iter)) != NULL) {
        char *temp = writeProperty(elem);
        property = realloc(property, sizeof(char) * (strlen(property) + strlen(temp) + 7));
        strcat(property, "\r\n");
        strcat(property, temp);
        free(temp);
    }
    char *string = malloc(sizeof(char) * (strlen(property) + strlen(alarm->action) + strlen(alarm->trigger) + 50));
    strcpy(string, "BEGIN:VALARM\r\nACTION:");
    strcat(string, alarm->action);
    strcat(string, "\r\nTRIGGER:");
    strcat(string, alarm->trigger);
    strcat(string, property);
    strcat(string, "\r\nEND:VALARM");
    free(property);
    return string;
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
    if (toBeDeleted == NULL) {
        return;
    }
    Alarm *alarm = (Alarm *) toBeDeleted;
    if (alarm->trigger != NULL) {
        free(alarm->trigger);
    }
    clearList(&alarm->properties);
    if (alarm != NULL) {
        free(alarm);
    }
}

char *printProperties(void *toBePrinted) {
    if (toBePrinted == NULL) {
        return NULL;
    }
    Property *aProperty = (Property *) toBePrinted;
    int length = strlen(aProperty->propName) + strlen(aProperty->propDescr) + 5;
    char *string = malloc(sizeof(char) * length);
    strcpy(string, "\t\t-");
    strcat(string, aProperty->propName);
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
    return strcasecmp(Property1->propName, Property2->propName);
}

void deleteProperties(void *toBeDeleted) {
    if (toBeDeleted == NULL) {
        return;
    }
    Property *aProperty = (Property *) toBeDeleted;
    free(aProperty);
}

int compareEvent(const void *first, const void *second) {
    if (first == NULL || second == NULL) {
        return 0;
    }
    Event *event1 = (Event *) first;
    Event *event2 = (Event *) second;
    return strcmp(event1->UID, event2->UID);
}

void deleteEvent(void *toBeDeleted) {
    if (toBeDeleted == NULL) {
        return;
    }
    Event *delete = (Event *) toBeDeleted;
    clearList(&delete->properties);
    clearList(&delete->alarms);
    if (delete != NULL) {
        free(delete);
    }
}

/**
 * travel all the properties in calendar object, find the property name and return the count of the property name.
 * if find some properties which name or value is empty string. return -1
 * @param obj calendar object
 * @param aProperty proprety which have name that need to find.
 * @return
 */
int propertyDupFind(List properties, const Property *aProperty) {
    ListIterator iter = createIterator(properties);
    void *elem;
    int findCount = 0;
    while ((elem = nextElement(&iter)) != NULL) {
        Property *temp = (Property *) elem;
        if (strlen(temp->propName) == 0 || strlen(temp->propDescr) == 0) {
            return -1;
        }
        if (compareProperties(aProperty, elem) == 0) {
            findCount++;
        }
    }
    return findCount;
}

//todo add x-prop and iana-prop validate
ICalErrorCode vCalValidate(const Calendar *obj) {
    if (obj->prodID == NULL) {
        return INV_CAL;
    }
    if (strlen(obj->prodID) <= 0) {
        return INV_PRODID;
    }
    if (obj->version <= 0) {
        return INV_VER;
    }
    if (obj->events.length == 0) {
        return INV_CAL;
    }
    //prodid dup valid
    Property *aProperty = malloc(sizeof(Property));
    strcpy(aProperty->propName, "prodid");
    if (propertyDupFind(obj->properties, aProperty) != 0) {
        deleteProperties(aProperty);
        return DUP_PRODID;
    } else if (propertyDupFind(obj->properties, aProperty) == -1) {
        deleteProperties(aProperty);
        return INV_CAL;
    }
    strcpy(aProperty->propName, "version");
    if (propertyDupFind(obj->properties, aProperty) != 0) {
        deleteProperties(aProperty);
        return DUP_VER;
    }
    strcpy(aProperty->propName, "calscale");
    if (propertyDupFind(obj->properties, aProperty) > 1) {
        deleteProperties(aProperty);
        return INV_CAL;
    }
    strcpy(aProperty->propName, "method");
    if (propertyDupFind(obj->properties, aProperty) > 1) {
        deleteProperties(aProperty);
        return INV_CAL;
    }
    deleteProperties(aProperty);
    void *elem;
    ListIterator iter = createIterator(obj->properties);
    while ((elem = nextElement(&iter)) != NULL) {
        Property *aProperty1 = (Property *) elem;
        if (strcasecmp(aProperty1->propName, "calscale") != 0 && strcasecmp(aProperty1->propName, "method") != 0) {
            return INV_CAL;
        }
    }
    return OK;
}

//todo add x-prop and iana-prop validate
ICalErrorCode vAlarmValidate(List alarm) {
    void *elem;
    ListIterator iter = createIterator(alarm);
    while ((elem = nextElement(&iter)) != NULL) {
        Alarm *temp = (Alarm *) elem;
        if (strlen(temp->action) == 0) {
            return INV_ALARM;
        }
        if (temp->trigger == NULL || strlen(temp->trigger) == 0) {
            return INV_ALARM;
        }
        Property *aProperty = malloc(sizeof(Property));
        strcpy(aProperty->propName, "action");
        if (propertyDupFind(temp->properties, aProperty) != 0) {
            deleteProperties(aProperty);
            return INV_ALARM;
        } else if (propertyDupFind(temp->properties, aProperty) == -1) {
            deleteProperties(aProperty);
            return INV_ALARM;
        }
        strcpy(aProperty->propName, "trigger");
        if (propertyDupFind(temp->properties, aProperty) != 0) {
            deleteProperties(aProperty);
            return INV_ALARM;
        }
        int durationCount = 0;
        int repeatCount = 0;
        strcpy(aProperty->propName, "duration");
        if ((durationCount = propertyDupFind(temp->properties, aProperty)) > 1) {
            deleteProperties(aProperty);
            return INV_ALARM;
        }
        strcpy(aProperty->propName, "repeat");
        if ((repeatCount = propertyDupFind(temp->properties, aProperty)) > 1) {
            deleteProperties(aProperty);
            return INV_ALARM;
        }
        if (durationCount != repeatCount) {
            return INV_ALARM;
        }
        if (strcasecmp(temp->action, "audio") == 0) {
            strcpy(aProperty->propName, "description");
            if (propertyDupFind(temp->properties, aProperty) != 0) {
                deleteProperties(aProperty);
                return INV_ALARM;
            }
        } else if (strcasecmp(temp->action, "display") == 0) {
            strcpy(aProperty->propName, "description");
            if (propertyDupFind(temp->properties, aProperty) != 1) {
                deleteProperties(aProperty);
                return INV_ALARM;
            }
        } else if (strcasecmp(temp->action, "email") == 0) {
            strcpy(aProperty->propName, "description");
            if (propertyDupFind(temp->properties, aProperty) != 1) {
                deleteProperties(aProperty);
                return INV_ALARM;
            }
            strcpy(aProperty->propName, "summary");
            if (propertyDupFind(temp->properties, aProperty) != 1) {
                deleteProperties(aProperty);
                return INV_ALARM;
            }

            strcpy(aProperty->propName, "attendee");
            if (propertyDupFind(temp->properties, aProperty) < 1) {
                deleteProperties(aProperty);
                return INV_ALARM;
            }
        }

        deleteProperties(aProperty);
        void *elem2;
        ListIterator iter2 = createIterator(temp->properties);
        while ((elem2 = nextElement(&iter2)) != NULL) {
            Property *aProperty1 = (Property *) elem2;
            if (strcasecmp(aProperty1->propName, "REPEAT") != 0
                && strcasecmp(aProperty1->propName, "ATTENDEE") != 0
                && strcasecmp(aProperty1->propName, "DURATION") != 0
                && strcasecmp(aProperty1->propName, "SUMMARY") != 0
                && strcasecmp(aProperty1->propName, "DESCRIPTION") != 0
                && strcasecmp(aProperty1->propName, "ATTACH") != 0) {
                return INV_EVENT;
            }
        }

    }
    return OK;
}


//todo add x-prop and iana-prop validate
ICalErrorCode vEventValidate(List event) {
    void *elem;
    ListIterator iter = createIterator(event);
    while ((elem = nextElement(&iter)) != NULL) {
        Event *temp = (Event *) elem;
        if (strlen(temp->UID) == 0) {
            return INV_EVENT;
        }
        if (strlen(temp->creationDateTime.date) != 8 || strlen(temp->creationDateTime.time) != 6) {
            return INV_CREATEDT;
        }
        Property *aProperty = malloc(sizeof(Property));
        strcpy(aProperty->propName, "uid");
        if (propertyDupFind(temp->properties, aProperty) != 0) {
            deleteProperties(aProperty);
            return INV_EVENT;
        } else if (propertyDupFind(temp->properties, aProperty) == -1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        strcpy(aProperty->propName, "dtstamp");
        if (propertyDupFind(temp->properties, aProperty) != 0) {
            deleteProperties(aProperty);
            return INV_CREATEDT;
        }
        strcpy(aProperty->propName, "dtstart");
        if (propertyDupFind(temp->properties, aProperty) > 1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        strcpy(aProperty->propName, "class");
        if (propertyDupFind(temp->properties, aProperty) > 1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        strcpy(aProperty->propName, "created");
        if (propertyDupFind(temp->properties, aProperty) > 1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        strcpy(aProperty->propName, "description");
        if (propertyDupFind(temp->properties, aProperty) > 1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        strcpy(aProperty->propName, "geo");
        if (propertyDupFind(temp->properties, aProperty) > 1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        strcpy(aProperty->propName, "last-mod");
        if (propertyDupFind(temp->properties, aProperty) > 1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        strcpy(aProperty->propName, "location");
        if (propertyDupFind(temp->properties, aProperty) > 1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        strcpy(aProperty->propName, "organizer");
        if (propertyDupFind(temp->properties, aProperty) > 1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        strcpy(aProperty->propName, "priority");
        if (propertyDupFind(temp->properties, aProperty) > 1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        strcpy(aProperty->propName, "seq");
        if (propertyDupFind(temp->properties, aProperty) > 1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        strcpy(aProperty->propName, "status");
        if (propertyDupFind(temp->properties, aProperty) > 1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        strcpy(aProperty->propName, "summary");
        if (propertyDupFind(temp->properties, aProperty) > 1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        strcpy(aProperty->propName, "transp");
        if (propertyDupFind(temp->properties, aProperty) > 1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        strcpy(aProperty->propName, "url");
        if (propertyDupFind(temp->properties, aProperty) > 1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        strcpy(aProperty->propName, "recurid");
        if (propertyDupFind(temp->properties, aProperty) > 1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        strcpy(aProperty->propName, "rrule");
        if (propertyDupFind(temp->properties, aProperty) > 1) {
            deleteProperties(aProperty);
            return INV_EVENT;
        }
        if (vAlarmValidate(temp->alarms) != OK) {
            deleteProperties(aProperty);
            return INV_ALARM;
        }
        deleteProperties(aProperty);
        void *elem2;
        ListIterator iter2 = createIterator(temp->properties);
        while ((elem2 = nextElement(&iter2)) != NULL) {
            Property *aProperty1 = (Property *) elem2;
            if (strcasecmp(aProperty1->propName, "attach") != 0
                && strcasecmp(aProperty1->propName, "CATEGORIES") != 0
                && strcasecmp(aProperty1->propName, "CLASS") != 0
                && strcasecmp(aProperty1->propName, "COMMENT") != 0
                && strcasecmp(aProperty1->propName, "DESCRIPTION") != 0
                && strcasecmp(aProperty1->propName, "GEO") != 0
                && strcasecmp(aProperty1->propName, "LOCATION") != 0
                && strcasecmp(aProperty1->propName, "PRIORITY") != 0
                && strcasecmp(aProperty1->propName, "RESOURCES") != 0
                && strcasecmp(aProperty1->propName, "STATUS") != 0
                && strcasecmp(aProperty1->propName, "SUMMARY") != 0
                && strcasecmp(aProperty1->propName, "DTEND") != 0
                && strcasecmp(aProperty1->propName, "DTSTART") != 0
                && strcasecmp(aProperty1->propName, "DURATION") != 0
                && strcasecmp(aProperty1->propName, "TRANSP") != 0
                && strcasecmp(aProperty1->propName, "CONTACT") != 0
                && strcasecmp(aProperty1->propName, "ORGANIZER") != 0
                && strcasecmp(aProperty1->propName, "RECURRENCE-ID") != 0
                && strcasecmp(aProperty1->propName, "RELATED-TO") != 0
                && strcasecmp(aProperty1->propName, "URL") != 0
                && strcasecmp(aProperty1->propName, "EXDATE") != 0
                && strcasecmp(aProperty1->propName, "RDATE") != 0
                && strcasecmp(aProperty1->propName, "RRULE") != 0
                && strcasecmp(aProperty1->propName, "RDATE") != 0
                && strcasecmp(aProperty1->propName, "CREATED") != 0
                && strcasecmp(aProperty1->propName, "LAST-MODIFIED") != 0
                && strcasecmp(aProperty1->propName, "SEQUENCE") != 0) {
                return INV_EVENT;
            }
        }
    }
    return OK;
}


char *foldWritenString(char *string) {
    if (string == NULL) {
        return NULL;
    }
    int charCount = 0;
    char *temp;
    for (int i = 0; i < (strlen(string) - 5); ++i) {
        charCount++;
        if (strncmp(string + i, "\r\n", 2) == 0) {
            charCount = 0;
        }
        if (charCount == 76) {
            temp = malloc(sizeof(char) * (strlen(string) + 4));
            strcpy(temp, "");
            strncat(temp, string, i);
            strcat(temp, "\r\n ");
            strcat(temp, string + i);
            free(string);
            string = temp;
            charCount = 0;
            i++;
        }
    }
    return string;
}


char *writeCalendarString(const Calendar *obj) {
    if (obj == NULL) {
        return NULL;
    }
    char *event = malloc(sizeof(char) * 10);
    strcpy(event, "");
    ListIterator iter = createIterator(obj->events);
    void *elem;
    while ((elem = nextElement(&iter)) != NULL) {
        char *temp = writeEvent(elem);
        event = realloc(event, sizeof(char) * (strlen(event) + strlen(temp) + 7));
        strcat(event, temp);
        strcat(event, "\r\n");
        free(temp);
    }
    char *version = malloc(sizeof(char) * 5);
    gcvt(obj->version, 2, version);
    char *string = malloc(sizeof(char) * (strlen(event) + strlen(obj->prodID) + strlen(version) + 50));
    strcpy(string, "BEGIN:VCALENDAR\r\nVERSION:");
    strcat(string, version);
    strcat(string, "\r\nPRODID:");
    strcat(string, obj->prodID);
    strcat(string, "\r\n");
    strcat(string, event);
    strcat(string, "END:VCALENDAR");
    free(event);
    free(version);
    return string;
}




