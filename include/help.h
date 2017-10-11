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
 * @return boolean, true for valid, false for invalid.
 */
bool fileNameCheck(char *fileName);

/**
 * get the file size.
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

/**
 * unfold the data in the memory.
 * @param listOfToken the token list that contain the foled string.
 * @return true for convert ok, false for convert failed.
 */
bool unFoldData(List *listOfToken);


/**
 * use regular expression to match the whole file in the List, if not match the requirement of the icalendar file
 * return error code, if meet the requirements, return OK.
 * @param listOfToken the list than contain the whole file.
 * @return the error code.
 */
ErrorCode fileValidation(List listOfToken);

/**
 * delete the elements in the list which contain a comment.
 * @param listOfToken the pointer of list that contain the whole file.
 */
void deleteCommont(List *listOfToken);

/**
 * according to the iterator element to identity the element if is a BEGIN:VCAL, END:VCAL, BEGIN:VALARM, END:VALARM....
 * @param elem the elem from the iterator.
 * @return the indicator number for switch to do different actions.
 */
int contentIndicator(void *elem);

/**
 *count how many VCALENDAR in this file.
 * @param listOfToken the list contain the whole file token by line.
 * @return the number of the VCALENDAR in the file.
 */
int countCalObject(List *listOfToken);

/**
 *create a string that contain the readable event list.
 * @param toBePrinted the pointer to an event which need to convert to a readable string.
 * @return a string that converted to readable event list.
 */
char *printEvent(void *toBePrinted);

/**
 * get the version number from the file. and convert to float type.
 * @param elem the elem which in the whole file list, token by line
 * @return the float number of the version.
 */
float getVersionNumber(void *elem);

/**
 *get the proddid from the file.
 * @param elem the elem which in the whole file list, token by line
 * @return the value of the prodid property.
 */
char *getProdid(void *elem);

/**
 * check if current element is END:VENET
 * @param elem the elem which in the whole file list, token by line
 * @return if yes, return true, else return false.
 */
bool isEndEvent(void *elem);

/**
 * get the UID from the file.
 * @param elem the elem which in the whole file list, token by line
 * @return a string which is the  the UID's value.
 */
char *getUID(void *elem);

/**
 * get the UIC date from the element.
 * @param elem the elem which in the whole file list, token by line
 * @return return a string which is the  the UTC date. need free.
 */
char *getUTCDate(void *elem);

/**
 * get the UTC time from the element.
 * @param elem the elem which in the whole file list, token by line
 * @return return a string which is the UTC time, need free.
 */
char *getUTCTime(void *elem);

/**
 * get the time from the element.
 * @param elem the elem which in the whole file list, token by line
 * @return return a string which is the time, need free.
 */
char *getTime(void *elem);

/**
 * check if current element is END:VALARM
 * @param elem the elem which in the whole file list, token by line
 * @return if yes, return true, else return false.
 */
bool isEndAlarm(void *elem);

/**
 * get the alarm action from the element.
 * @param elem the elem which in the whole file list, token by line
 * @return return a string which is the alarm action's value, need free.
 */
char *getAlarmAction(void *elem);

/**
 * get the alarm trigger value from the element.
 * @param elem the elem which in the whole file list, token by line
 * @return return a string which is the alarm trigger's value, need free.
 */
char *getAlarmTri(void *elem);

/**
 * get the properties name from the elemnet.
 * @param elem the elem which in the whole file list, token by line
 * @return return a string which is the properies' name.
 */
char *getName(void *elem);

/**
 * get the properties value from the element.
 * @param elem the elem which in the whole file list, token by line
 * @return return a string which is the properties' value.
 */
char *getDescription(void *elem);

char *printToken(void *toBePrinted);

int compartToken(const void *first, const void *second);

void deleteToken(void *toBeDeleted);

char *printAlarm(void *toBePrinted);

int comparAlarm(const void *first, const void *second);

void deleteAlarm(void *toBeDeleted);

char *printProperties(void *toBePrinted);

int compareProperties(const void *first, const void *second);

void deleteProperties(void *toBeDeleted);

int compareEvent(const void *first, const void *second);

void deleteEvent(void *toBeDeleted);

/**
 * if required elements for VCAL is not valid, return false. else return true.
 * @param obj
 * @return
 */
ErrorCode vCalValidate(const Calendar *obj);

ErrorCode vEventValidate(List event);