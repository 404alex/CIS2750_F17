/**
 * CIS2750 F2017
 * Assignment 0
 * Chenxingyu Chen 0951136
 * Description: This file is created by Chenxingyu Chen. This file implement the basic ADT link list function.
 * And some errors will cause the program terminated:
 * 1.operate the list when the list is NULL or empty.
 * 2.use the iterator when the iter is NULL.
 * 3.pass NULL to the necessary function point.
 *
 * And some other errors will be handle like:
 * 1. pass NULL point in the insertFront, insertBack and insertSorted function as toBeAdded will make the list skip
 * this data and not create the node.
 *
 * This implementation is based on the List API that I implemented in my Assignment 2 for CIS2520
 * Which I took in the F2016 with professor Fangju Wang. The permission to use my CIS2520 code in CIS2750
 * was obtained on my behalf by my current CIS2750 professor, Denis Nikitenko.
 */

#include <stdio.h>
#include <stdlib.h>
#include "LinkedListAPI.h"

/**
 * verify whether the list exist.
 * if the list not exists, terminate the program.
 * @param list pointer of the list.
 */
int listVerification(List *list) {
    if (list == NULL) {
        return 0;
    }
    return 1;
}

/**
 * verifiy wheather the head and tail point to a NULL.
 * if the head or tail is NULL, terminate the program.
 * @param list pointer of the list.
 */
int headTailVerification(List *list) {
    if (list->head == NULL || list->tail == NULL) {
        return 0;
    }
    return 1;
}

/**
 * verify whether the iterator exist.
 * if the iterator not exists, terminate the program.
 * @param iter pointer of the iterator.
 */
void iterVerification(ListIterator *iter) {
    if (iter == NULL) {
        printf("Fatal error, iterator not exits, terminating");
        exit(0);
    }
}

/**
 * data verification,
 * @param data pointer of the data.
 * @return if the data is not NULL, return true. if the data is NULL, return false.
 */
bool dataVerification(void *data) {
    return data != NULL;
}


void insertBack(List *list, void *toBeAdded) {
    if (listVerification(list) == 0) {
        return;
    }
    Node *nodeAdded = initializeNode(toBeAdded);
    if (nodeAdded == NULL) {
        return;
    }
    if (list->tail == NULL && list->head == NULL) {
        list->tail = list->head = nodeAdded;
        list->length++;
        return;
    }
    nodeAdded->previous = list->tail;
    list->tail->next = nodeAdded;
    list->tail = nodeAdded;
    list->length++;
    list->tail->next = NULL;
}

void insertFront(List *list, void *toBeAdded) {
    if (listVerification(list) == 0) {
        return;
    }
    Node *nodeAdded = initializeNode(toBeAdded);
    if (nodeAdded == NULL) {
        return;
    }
    if (list->tail == NULL && list->head == NULL) {
        list->length++;
        list->tail = list->head = nodeAdded;
        return;
    }
    nodeAdded->previous = NULL;
    nodeAdded->next = list->head;
    list->head->previous = nodeAdded;
    list->head = nodeAdded;
    list->length++;
}

Node *initializeNode(void *data) {
    if (!dataVerification(data)) {
        printf("\nData not legal, skip this data.\n");
        return NULL;
    }
    Node *node = malloc(sizeof(Node));
    node->data = NULL;
    node->previous = NULL;
    node->next = NULL;
    node->data = data;
    return node;
}

List initializeList(char *(*printFunction)(void *toBePrinted), void (*deleteFunction)(void *toBeDeleted),
                    int (*compareFunction)(const void *first, const void *second)) {
    if (printFunction == NULL || deleteFunction == NULL || compareFunction == NULL) {
        printf("Fatal error: Necessary Function Point is NULL. Terminating");
        exit(0);
    }
    List list;
    list.compare = compareFunction;
    list.deleteData = deleteFunction;
    list.printData = printFunction;
    list.head = NULL;
    list.tail = NULL;
    list.length = 0;
    return list;
}

ListIterator createIterator(List list) {
    listVerification(&list);
    ListIterator iter;
    iter.current = (&list)->head;
    return iter;
}

void *nextElement(ListIterator *iter) {
    iterVerification(iter);
    Node *current = iter->current;
    if (current == NULL) {
        return NULL;
    }
    iter->current = iter->current->next;
    return current->data;
}

void clearList(List *list) {
    //clear list, three situations.
    //1st, the head = tail = NULL
    //2nd, the head = tail != NULL
    //3rd. the head != tail
    list->length = 0;
    if (listVerification(list) == 0) {
        return;
    }
    //head = tail = NULL
    if (list->head == list->tail && list->tail == NULL) {
        return;
    }
    //head != tail
    if (list->tail->previous != NULL) {
        list->tail = list->tail->previous;
        list->deleteData(list->tail->next->data);
        free(list->tail->next);
        list->tail->next = NULL;
        clearList(list);
    } else {
        // head = tail != NULL
        list->deleteData(list->tail->data);
        free(list->tail);
        list->tail = NULL;
        list->head = NULL;
    }

}

void *getFromFront(List list) {
    if (listVerification(&list) == 0) {
        return NULL;
    }
    if (headTailVerification(&list) == 0) {
        return NULL;
    }
    return list.head->data;
}

void *getFromBack(List list) {
    if (listVerification(&list) == 0) {
        return NULL;
    }
    if (headTailVerification(&list) == 0) {
        return NULL;
    }
    return list.tail->data;
}

/**
 * in this function, the ideal of separate by '; ' is come from the in-class example.
 * @param list
 * @return
 */
char *toString(List list) {
    listVerification(&list);
    void *elem;
    ListIterator iter = createIterator(list);
    char *string = malloc(sizeof(char) * 2);    //create a new string.
    strcpy(string, "");     // initialize string.
    int length = 2;   //count the lenght of the string.
    while ((elem = nextElement(&iter)) != NULL) {
        char *str = list.printData(elem);
        length += (strlen(str) + 4);
        string = realloc(string, sizeof(char) * length);    //re-allocate the mem by the string length.
        strcat(string, str);    //cat the string to the end of the string.
        free(str);          //after done, free the string.
        strcat(string, "; ");   // separate by '; '
    }
    return string;
    // if just use printf(toString(list)) will cause the memory leak.
}

void *deleteDataFromList(List *list, void *toBeDeleted) {
    if (listVerification(list) == 0) {
        return NULL;
    }
    if (toBeDeleted == NULL) {
        return NULL;
    }
    if (list->head == NULL && list->tail == NULL) {
        return NULL;
    }
    void *elem;
    ListIterator iter = createIterator(*list);
    while ((elem = nextElement(&iter)) != NULL) {
        //delete the specific node in the list.
        if (list->compare(toBeDeleted, elem) == 0) {   //find the node.
            //if the toBeDeleted node is on the list
            //there are three situations
            //1st, the node at the begin.
            //2nd, the node at the end
            //3rd, the node at the middle.

            //head = tail
            if (list->head == list->tail && list->head != NULL) {
                void *data = elem;
                free(list->head);
                list->head = NULL;
                list->tail = NULL;
                list->length--;
                return data;
            } else if (iter.current == NULL) {
                //node at the end
                list->tail = list->tail->previous;
                void *data = elem;
                free(list->tail->next);
                list->tail->next = NULL;
                list->length--;
                return data;
            } else if (iter.current->previous->previous == NULL) {
                //node at the begin
                list->head = list->head->next;
                void *data = elem;
                free(list->head->previous);
                list->head->previous = NULL;
                list->length--;
                return data;
            } else {
                //node at the middle
                Node *tempNode = iter.current->previous;
                iter.current->previous = iter.current->previous->previous;
                iter.current->previous->next = iter.current;
                void *data = elem;
                free(tempNode);
                list->length--;
                return data;
            }
        }
    }
    return NULL;
}


void insertSorted(List *list, void *toBeAdded) {
    //insert into a sorted list.
    //two situations: 1st, low to high sorted, 2nd, high to low sorted.

    if (listVerification(list) == 0) {
        return;
    }
    if (list->tail == NULL && list->head == NULL) {
        Node *nodeAdded = initializeNode(toBeAdded);
        if (nodeAdded == NULL) {
            return;
        }
        list->length++;
        list->tail = list->head = nodeAdded;
        return;
    }
    if (list->compare(list->head->data, list->tail->data) == 0) {
        insertBack(list, toBeAdded);
    } else if (list->compare(list->head->data, list->tail->data) > 0) {
        //high to low
        void *elem;
        ListIterator iter = createIterator(*list);
        int flag = 0;
        while ((elem = nextElement(&iter)) != NULL) {
            if (list->compare(toBeAdded, elem) >= 0) {
                if (elem == list->head->data) {
                    insertFront(list, toBeAdded);
                    flag++;
                    return;
                } else {
                    Node *node = initializeNode(toBeAdded);
                    if (node == NULL) {
                        return;
                    }
                    if (iter.current == NULL) {
                        Node *currentElemNode = list->tail;
                        node->previous = currentElemNode->previous;
                        node->next = currentElemNode;
                        currentElemNode->previous = node;
                        node->previous->next = node;
                        list->length++;
                        flag++;
                        return;
                    } else if (iter.current->previous != NULL) {
                        Node *currentElemNode = iter.current->previous;
                        node->previous = currentElemNode->previous;
                        node->next = currentElemNode;
                        currentElemNode->previous = node;
                        node->previous->next = node;
                        list->length++;
                        flag++;
                        return;
                    }
                }

            }
        }
        if (flag == 0) {
            insertBack(list, toBeAdded);
        }
    } else {
        //low to high
        void *elem;
        int flag = 0;
        ListIterator iter = createIterator(*list);
        while ((elem = nextElement(&iter)) != NULL) {
            if (list->compare(toBeAdded, elem) <= 0) {
                if (elem == list->head->data) {
                    insertFront(list, toBeAdded);
                    flag++;
                    return;
                } else {
                    Node *node = initializeNode(toBeAdded);
                    if (node == NULL) {
                        return;
                    }
                    if (iter.current == NULL) {
                        Node *currentElemNode = list->tail;
                        node->previous = currentElemNode->previous;
                        node->next = currentElemNode;
                        currentElemNode->previous = node;
                        node->previous->next = node;
                        list->length++;
                        flag++;
                        return;
                    } else if (iter.current->previous != NULL) {
                        Node *currentElemNode = iter.current->previous;
                        node->previous = currentElemNode->previous;
                        node->next = currentElemNode;
                        currentElemNode->previous = node;
                        node->previous->next = node;
                        list->length++;
                        flag++;
                        return;
                    }
                }

            }
        }
        if (flag == 0) {
            insertBack(list, toBeAdded);
        }
    }
}


int getLength(List list) {
    if (listVerification(&list) == 0) {
        return -1;
    }
    return list.length;
}


void *findElement(List list, bool (*customCompare)(const void *first, const void *second), const void *searchRecord) {
    if (listVerification(&list) == 0) {
        return NULL;
    }
    if (headTailVerification(&list) == 0) {
        return NULL;
    }
    if (customCompare == NULL) {
        return NULL;
    }
    if (searchRecord == NULL) {
        return NULL;
    }
    void *elem;
    ListIterator iter = createIterator(list);
    while ((elem = nextElement(&iter)) != NULL) {
        if (customCompare((const void *) elem, (const void *) searchRecord)) {
            return elem;
        }
    }
    return NULL;
}




