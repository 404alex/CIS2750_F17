#include "LinkedListAPI.h"
#include "CalendarParser.h"

#ifndef CIS2750A0_HELP_H
#define CIS2750A0_HELP_H

#endif //CIS2750A0_HELP_H

/**
 * string to lower case.
 * @param string the string pointer which need to be lower case.
 * @return a string pointer already convert to lower case.
 */
char *stringToLower(char *string);

/**
 * check the file name legal or not. Must contain .ics
 * @param fileName the path of the file.
 * @return
 */
bool fileNameCheck(char *fileName);

/**
 * http://blog.csdn.net/zdyueguanyun/article/details/51392736
 * @param file file point
 * @return the length of the file.
 */
int fileSize(FILE *file);


/**
 * read the whole file and return a string, the string is dynamic allocate, so need free!
 * @param file file point
 * @return the string of whole file, need free.
 */
char *readIntoBuffer(FILE *file);


/**
 * insert tokenized string to the list
 * @param icsFile string which need to be tokenized
 * @param listOfToken a list to storage the tokenized string.
 */
void insertTokenizedList(const char *icsFile, List *listOfToken);


bool unFoldData(List *listOfToken);


/**
 * http://c.biancheng.net/cpp/html/1428.html
 * @param listOfToken
 * @return
 */
ErrorCode fileValidation(List listOfToken);


void deleteCommont(List *listOfToken);

int contentIndicator(void *elem);

int countCalObject(List *listOfToken);

char *printEvent(Event *toBePrinted);

float getVersionNumber(void *elem);

char *getProdid(void *elem);

bool isEndEvent(void *elem);

char *getUID(void *elem);

char *getUTCDate(void *elem);

char *getUTCTime(void *elem);

char *getTime(void *elem);

bool isEndAlarm(void *elem);

char *getAlarmAction(void *elem);

char *getAlarmTri(void *elem);

char *getName(void *elem);

char *getDescription(void *elem);