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
unsigned int listAdd(listObject *instance, void *ptr);

/**
 * Removes an element from the list
 */
void listDel(listObject *instance, const unsigned int id);

/**
 * Return the current element
 */
void *listCurrent(const listObject *instance);

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
unsigned int listValid(const listObject *instance);

/**
 * Return the number of elements in the list
 */
unsigned int listCount(const listObject *instance);

#endif /* LIST_H */