
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
 * moves - list of the cells that changed in command
 * next - the next step
 * prev - the prev step
 */

typedef struct Step {
    Move* moves;
    struct Step* next;
    struct Step* prev;
} Step;

void addToDoublyList(Step** head_ref, Move* movesList);

void deleteNode(Step** head_ref, Step* del);

void printList(Step* node);

void deleteAllNextNodes(Step** head_ref, Step* node);

#endif /* DOUBLYLINKEDLIST_H_ */
