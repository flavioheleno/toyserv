#include <stdlib.h>
#include <string.h>

#include "list.h"

typedef struct _listElement {
    int id;
    void *ptr;
    struct _listElement *next;
} listElement;

struct _listObject {
    listElement *head;
    listElement *curr;
    int elementCount;
    int idSequence;
};

listObject *listConstruct() {
    listObject *instance = (listObject *)malloc(sizeof(listObject));
    instance->head = NULL;
    instance->curr = NULL;
    instance->elementCount = 0;
    instance->idSequence = 0;

    return instance;
}

void listDestruct(listObject *instance) {
    listRewind(instance);
    while (listValid(instance)) {
        listElement *element = listCurrent(instance);
        listNext(instance);
        listDel(instance, element->id);
    }
}

int listAdd(listObject *instance, void *ptr) {
    listElement *element = (listElement *)malloc(sizeof(listElement));
    memset(element, 0, sizeof(listElement));

    element->id = ++instance->idSequence;
    element->ptr = ptr;
    element->next = instance->head;
    instance->head = element;

    if (instance->curr == NULL) {
        instance->curr = instance->head;
    }

    instance->elementCount++;

    return instance->idSequence;
}

void listDel(listObject *instance, int id) {
    listElement *curr = instance->head;
    listElement *prev = NULL;

    /* id is at instance's head position */
    if (curr != NULL && curr->id == id) {
        /* internal pointer update */
        if (instance->curr == curr) {
            instance->curr = curr->next;
        }

        instance->head = curr->next;
        instance->elementCount--;
        free(curr);

        return;
    }

    /* traverse list searching for the element with target id */
    while (curr != NULL && curr->id != id) {
        prev = curr;
        curr = curr->next;
    }

    /* id was not present in the list */
    if (curr == NULL) {
        return;
    }

    /* internal pointer update */
    if (instance->curr == curr) {
        instance->curr = curr->next;
    }

    /* handle removal */
    prev->next = curr->next;
    instance->elementCount--;
    free(curr);
}

void *listCurrent(listObject *instance) {
    return instance->curr->ptr;
}

void listNext(listObject *instance) {
    instance->curr = instance->curr->next;
}

void listRewind(listObject *instance) {
    instance->curr = instance->head;
}

int listValid(listObject *instance) {
    return instance->curr != NULL;
}

int listCount(listObject *instance) {
    return instance->elementCount;
}