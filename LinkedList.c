#include "LinkedList.h"

void insertAfterDoubly(DoublyLinkedList *current, DoublyLinkedList *new) {
	if (current!=NULL){
		new->next = current->next;
		current->next->prev = new;

		new->prev = current;
		current->next = new;
	}
}

void deleteCurrentDoubly(DoublyLinkedList *current) {
	if (current->next!=current) {
		current->next->prev = current->prev;
		current->prev->next = current->next;
	}

	free(current);
}

void insertAfterSingle(SingleLinkedList *current, SingleLinkedList *new) {
	new->next = current->next;
	current->next = new;
}

bool deleteCurrentSingle(SingleLinkedList *current, SingleLinkedList *head) {
	while (head->next != NULL && head->next == current) {
		head = head->next;
	}
	if (head->next == NULL)
		return false;
	else if (head->next == current)
		head->next = current->next;

	free(current);

	return true;
}
