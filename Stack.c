#include "Stack.h"

void init(Stack *stk) {
	stk->size = 0;
	stk->top = NULL;
}

bool push(Puzzle *puzzle, int col, int row, Stack *stk) {
	StackNode *p = NULL;
	int *values = (int*)calloc(puzzle->blockNumOfCells+1, sizeof(int));
	p = (StackNode*)calloc(1, sizeof(StackNode));
	if (p != NULL) { /* otherwise calloc failed */
		p->col = col;
		p->row = row;
		p->options = numOfCellSol(puzzle, col, row, values);
		p->next = stk->top;
		stk->top = p;
		stk->size++;
		return true;
	}
	else
		return false;
}

int pop(Stack *stk) {
	int option;
	StackNode *p = NULL;

	option = stk->top->options[0];
	free(stk->top->options);
	p = stk->top;
	stk->top = stk->top->next;
	stk->size--;
	free(p);

	return option;
}

int topCol(Stack *stk) {
	if (stk->top == NULL)
	{
		return 0;
	}
	return (stk->top->col);
}

int topRow(Stack *stk) {
	if (stk->top == NULL)
	{
		return 0;
	}
	return (stk->top->row);
}

int topOption(Stack *stk) {
	return (stk->top->options[0]);
}

bool isEmpty(Stack *stk) {
	return (stk->size == 0 ? true : false);
}
