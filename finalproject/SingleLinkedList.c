#include "SingleLinkedList.h"

/* 
 * Single Linked List Module - Source
 * implement all single-linked-list operations
 */

/*
 * insert new single-linked-list node to the start of the list that begins at head
 * each move contains the coords of the cell that changed and its old and new values
 * pre-condition: in first call *head=NULL
 */
void addToList(Move ** head, int x, int y, int oldValue, int newValue) {
    Move * new_node;
    new_node = (Move*)malloc(sizeof(Move));

    new_node->x = x;
    new_node->y=y;
    new_node->oldValue=oldValue;
    new_node->newValue=newValue;
    new_node->next = *head;
    *head = new_node;
}

/* print the list that begins at head */
void print_list(Move * head) {
	Move * current = head;

    while (current != NULL) {
        printf("  <%d,%d>: %d -> %d\n", current->x, current->y, current->oldValue, current->newValue);
        current = current->next;
    }

    printf("NULL\n");
}

/* delete all the list that begins at head */
void deleteList(Move *head) {
	Move  *current = head,
            *next = head;

    while (current) {
        next = current->next;
		free(current);
        current = next;
    }
}

/* concat 2 single-linked-lists */
void concat(Move** head1, Move** head2) {
	Move *current=*head1;
	if (head1!=NULL && head2 != NULL) {
		if (*head2!=NULL)
		{
			if (*head1==NULL)
			{
				*head1=*head2;
				return;
			}
			else
			{
				while(current->next!=NULL) {
					current = current->next;
				}
				current->next=*head2;
			}
		}
	}
	else if ((head1==NULL) && (head2!=NULL)) {
		head1=head2;
	}
}

