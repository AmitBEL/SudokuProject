#include "Stack.h"

void init(Stack *stk) {
	stk->size = 0;
	stk->top = NULL;
}

bool push(int value, Stack *stk) {
	StackNode *p = NULL;

	p = malloc(sizeof(StackNode));
	if (p != NULL) { /* otherwise malloc failed */
		p->value = value;
		p->next = stk->top;
		stk->top = p;
		stk->size++;
		return true;
	}
	else
		return false;
}

int pop(Stack *stk) {
	int value;
	StackNode *p = NULL;

	value = stk->top->value;
	p = stk->top;
	stk->top = stk->top->next;
	stk->size--;
	free(p);

	return value;
}

int top(Stack *stk) {
	return (stk->top->value);
}

bool isEmpty(Stack *stk) {
	return (stk->size == 0 ? true : false);
}

bool isFull(Stack *stk) {
	return (stk->size == MAX_SIZE ? true : false);
}
