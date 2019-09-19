
#ifndef STACK_H_
#define STACK_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Auxilary.h"

/* 
 * Stack Module - Header
 * define stack node and stack
 * manage all stack operations
 */

/*
 * define stack node
 * col, row - coords of current cell in backtracking
 * options - array with 1 at index of possible value
 * next - pointer to next node in stack
 */
typedef struct StackNode
{
	int col;
	int row;
	int *options;
	struct StackNode *next;
} StackNode;

/*
 * define stack
 * size - size of stack
 * top - pointer to the top of the stack
 */
typedef struct Stack
{
	int size;
	StackNode *top;
} Stack;

void init(Stack *stk);

bool push(Puzzle *puzzle, int col, int row, Stack *stk);

int pop(Stack *stk);

int topCol(Stack *stk);

int topRow(Stack *stk);

int topOption(Stack *stk);

bool isEmpty(Stack *stk);

#endif
