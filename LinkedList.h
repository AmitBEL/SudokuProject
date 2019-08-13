#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* doubly part */

typedef struct DoublyLinkedList {
   int value;
   struct DoublyLinkedList *prev;
   struct DoublyLinkedList *next;
} DoublyLinkedList;

/*
 * pre-condition: current=NULL iff it's a new list
 */
void insertAfterDoubly(DoublyLinkedList *current, DoublyLinkedList *new);

void deleteCurrentDoubly(DoublyLinkedList *current);

/* single part */

typedef struct SingleLinkedList {
   int value;
   struct SingleLinkedList *next;
} SingleLinkedList;

void insertAfterSingle(SingleLinkedList *current, SingleLinkedList *new);

/*
 * return true if current found in list and deleted
 */
bool deleteCurrentSingle(SingleLinkedList *current, SingleLinkedList *head);

#endif
