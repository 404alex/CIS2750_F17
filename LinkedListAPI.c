#include <stdio.h>
#include <stdlib.h>
#include "LinkedListAPI.h"

void listVerification(List *list) {
    if (list == NULL) {
        printf("Fatal error, list not exists, exiting");
        exit(0);
    }
}

void iterVerification(ListIterator *iter) {
    if (iter == NULL) {
        printf("Fatal error, iterator not exits, exiting");
        exit(0);
    }
}

bool isDataSameSize(void *data) {
    return sizeof(data) == sizeof(void);
}

bool isDataVaild(void *data) {
    return data != NULL;
}

bool dataVerification(void *data) {
    return /*isDataSameSize(data) &&*/ isDataVaild(data);
}


void insertBack(List *list, void *toBeAdded) {
    listVerification(list);
    Node *nodeAdded = initializeNode(toBeAdded);
    if (nodeAdded == NULL) {
        return;
    }
    if (list->tail == NULL && list->head == NULL) {
        list->tail = list->head = nodeAdded;
        return;
    }
    nodeAdded->previous = list->tail;
    list->tail->next = nodeAdded;
    list->tail = nodeAdded;
    list->tail->next = NULL;
}

void insertFront(List *list, void *toBeAdded) {
    listVerification(list);
    Node *nodeAdded = initializeNode(toBeAdded);
    if (nodeAdded == NULL) {
        return;
    }
    nodeAdded->previous = NULL;
    nodeAdded->next = list->head;
    list->head->previous = nodeAdded;
    list->head = nodeAdded;
}

Node *initializeNode(void *data) {
    if (!dataVerification(data)) {
        printf("Data not legal, skip this data.");
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
    List list; //= malloc(sizeof(List));
    list.compare = compareFunction;
    list.deleteData = deleteFunction;
    list.printData = printFunction;
    list.head = NULL;
    list.tail = NULL;
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
        //free(iter);
        return NULL;
    }
    iter->current = iter->current->next;
    return current->data;
}

void clearList(List *list) {
    listVerification(list);
    if (list->head == list->tail && list->tail == NULL) {
        return;
    }
    if (list->tail->previous != NULL) {
        list->tail = list->tail->previous;
        list->deleteData(list->tail->next->data);
        free(list->tail->next);
        clearList(list);
    } else {
        list->deleteData(list->tail->data);
        free(list->tail);
    }

}

void *getFromFront(List *list) {
    listVerification(list);
    return list->head;
}

void *getFromBack(List *list) {
    listVerification(list);
    return list->tail;
}


