#include "DoublyLinkedList.h"

/* 
 * Doubly Linked List Module - Source
 * implement all doubly-linked-list operations
 */


/* add new node of movesList to the end of the list that begins at head_ref */
void addToDoublyList(Step** head_ref, Move* movesList)
{
    Step* new_node = (Step*)malloc(sizeof(Step));

    Step* last = *head_ref; /* used in step 5*/

    new_node->moves = movesList;
    new_node->next = NULL;

    if (*head_ref == NULL) {
        new_node->prev = NULL;
        *head_ref = new_node;
        return;
    }

    while (last->next != NULL)
        last = last->next;

    last->next = new_node;
    new_node->prev = last;
}

/* delete the node del from the list that begins at head_ref */
void deleteNode(Step** head_ref, Step* del)
{
    if (*head_ref == NULL || del == NULL)
        return;

    deleteList(del->moves);

    if (*head_ref == del)
        *head_ref = del->next;

    if (del->next != NULL)
        del->next->prev = del->prev;

    if (del->prev != NULL)
        del->prev->next = del->next;

    free(del);
    return;
}

/* delete all next nodes after node in the list that begins at head_ref */
void deleteAllNextNodes(Step** head_ref, Step* node) {
	if (*head_ref == NULL || node == NULL)
	        return;

	while (node->next!=NULL) {
		deleteNode(head_ref, node->next);
	}
}

/* print content of linked list that begins at node */
void printList(Step* node)
{
    while (node != NULL) {
        printf("\n<%d,%d>:\n", node->moves->x, node->moves->y);
        print_list(node->moves);
        node = node->next;
    }

    printf("NULL\n");
}

