#ifndef LIST_H
#define LIST_H

typedef struct _listObject listObject;

/**
 * Class constructor
 */
listObject *listConstruct();

/**
 * Class destructor
 */
void listDestruct(listObject *instance);

/**
 * Adds a new element to the beginning of the list
 */
int listAdd(listObject *instance, void *ptr);

/**
 * Removes an element from the list
 */
void listDel(listObject *instance, int id);

/**
 * Return the current element
 */
void *listCurrent(listObject *instance);

/**
 * Move forward to next element
 */
void listNext(listObject *instance);

/**
 * Rewind the Iterator to the first element
 */
void listRewind(listObject *instance);

/**
 * Checks if current position is valid
 */
int listValid(listObject *instance);

/**
 * Return the number of elements in the list
 */
int listCount(listObject *instance);

#endif /* LIST_H */