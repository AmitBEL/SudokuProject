/*
 * how to use:
 * 1. create Stack object s
 * 2. call init(&s)
 * 3. check !isFull before calling push
 * 4. check !isEmpty before calling pop
 * 5. notice: always use a pointer to Stack object as argument
 */

#ifndef STACK_H_
#define STACK_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_SIZE 10000

typedef struct StackNode
{
	int value;
	struct StackNode *next;
} StackNode;

typedef struct Stack
{
	int size; /* size of stack */
	StackNode *top;
} Stack;

void init(Stack *stk);

bool push(int value, Stack *stk);

int pop(Stack *stk);

int top(Stack *stk);

bool isEmpty(Stack *stk);

bool isFull(Stack *stk);

#endif
