#include "LinkedListAPI.h"
#include "CalendarParser.h"

#ifndef CIS2750A0_HELP_H
#define CIS2750A0_HELP_H

#endif //CIS2750A0_HELP_H

char *stringToLower(char *string);

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


char *unFlodICSFile(char *icsFile);


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