#include "DoublyLinkedList.h"

/* 
 * Doubly Linked List Module - Source
 * implement all doubly-linked-list operations
 */


/* Given a reference (pointer to pointer) to the head
   of a DLL and an int, appends a new node at the end  */
void addToDoublyList(Step** head_ref, Move* movesList)
{
    /* 1. allocate node */
    Step* new_node = (Step*)malloc(sizeof(Step));

    Step* last = *head_ref; /* used in step 5*/
    /*printf("34. malloc Step *firstNode<%d,%d> - addToDoublyList, DoublyLinkedList\n", movesList->x, movesList->y);*/

    /* 2. put in the data  */
    new_node->moves = movesList;

    /* 3. This new node is going to be the last node, so
          make next of it as NULL*/
    new_node->next = NULL;

    /* 4. If the Linked List is empty, then make the new
          node as head */
    if (*head_ref == NULL) {
        new_node->prev = NULL;
        *head_ref = new_node;
        return;
    }

    /* 5. Else traverse till the last node */
    while (last->next != NULL)
        last = last->next;

    /* 6. Change the next of last node */
    last->next = new_node;

    /* 7. Make last node as previous of new node */
    new_node->prev = last;
}

/* Function to delete a node in a Doubly Linked List.
   head_ref --> pointer to head node pointer.
   del  -->  pointer to node to be deleted. */
void deleteNode(Step** head_ref, Step* del)
{
    /* base case */
    if (*head_ref == NULL || del == NULL)
        return;

    /*printf("34. free Step *firstNode<%d,%d> - deleteNode, DoublyLinkedList\n", del->moves->x, del->moves->y);*/
    deleteList(del->moves);

    /* If node to be deleted is head node */
    if (*head_ref == del)
        *head_ref = del->next;

    /* Change next only if node to be deleted is NOT the last node */
    if (del->next != NULL)
        del->next->prev = del->prev;

    /* Change prev only if node to be deleted is NOT the first node */
    if (del->prev != NULL)
        del->prev->next = del->next;

    /* Finally, free the memory occupied by del*/
    
    free(del);
    return;
}

/* Function to delete all next nodes after node in a Doubly Linked List.
   head_ref --> pointer to head node pointer.
   node  -->  pointer to node that all its next nodes will be deleted. */
void deleteAllNextNodes(Step** head_ref, Step* node) {
	if (*head_ref == NULL || node == NULL)
	        return;

	while (node->next!=NULL) {
		deleteNode(head_ref, node->next);
	}
}

/* This function prints contents of linked list starting from the given node */
void printList(Step* node)
{
    while (node != NULL) {
        printf("\n<%d,%d>:\n", node->moves->x, node->moves->y);
        print_list(node->moves);
        node = node->next;
    }

    printf("NULL\n");
}

