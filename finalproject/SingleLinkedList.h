#ifndef SINGLELINKEDLIST_H_
#define SINGLELINKEDLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* 
 * Single Linked List Module - Header
 * define Move (single-linked-list node)
 * manage all single-linked-list operations
 */

/*
 * Move is node of Singly-linked-list and contains info about the cell that changed in command
 * x,y - coords of the cell that changed
 * oldValue - the old value of the cell that changed
 * newValue - the new value of the cell that changed
 */
typedef struct Move {
	int x;
	int y;
	int oldValue;
	int newValue;
    struct Move * next;
} Move;

void addToList(Move ** head, int x, int y, int oldValue, int newValue);

void print_list(Move * head);

void deleteList(Move *head);

void concat(Move** head1, Move** head2);

#endif
