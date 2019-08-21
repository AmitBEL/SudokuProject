
#ifndef DOUBLYLINKEDLIST_H_
#define DOUBLYLINKEDLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SingleLinkedList.h"

/* 
 * Doubly Linked List Module - Header
 * define Step (doubly-linked-list node)
 * manage all doubly-linked-list operations
 */

typedef struct Step {
    Move* moves;
    struct Step* next; /* Pointer to next node in DLL */
    struct Step* prev; /* Pointer to previous node in DLL */
} Step;

void addToDoublyList(Step** head_ref, Move* movesList);

void deleteNode(Step** head_ref, Step* del);

void printList(Step* node);

void deleteAllNextNodes(Step** head_ref, Step* node);

#endif /* DOUBLYLINKEDLIST_H_ */
