#include "Solver.h"

/* 
 * Solver Module - Source
 * implement the exhaustive backtrack algorithm 
 */


/* private functions declaration */
int choice(int *options, int size);

int nextCellCol(int col, int blockNumOfCells);

int nextCellRow(int col, int row, int blockNumOfCells);


/* choose the first index in range [1,size) such that options[index]==1
 * if there isn't such index return 0
 * pre: options length is size 
 */
int choice(int *options, int size)
{
	int i;
	for (i=1; i<size; i++)
	{
		if (options[i])
		{
			return i;
		}
	}
	return 0;
}

/* return the next cell col according to the current col */
int nextCellCol(int col, int blockNumOfCells)
{
	return ((col%blockNumOfCells)+1);
}

/* return the next cell row according to the current cell */
int nextCellRow(int col, int row, int blockNumOfCells)
{
	if (col%blockNumOfCells==0)
	{
		return (row+1);
	}
	else
	{
		return row;
	}
	
}

/* 
 * return the number of possible solutions to puzzle 
 * using the exhaustive backtrack algorithm
 * and a stack that simulate recursion
 */
int backtrack(Puzzle *puzzle)
{
	int c, col=0, row=0, cnt=0;
	Cell dummyCell={0,0,0};
	Cell *cell=&dummyCell;
	bool success=true, found=false, end = false;
	Stack *stk = (Stack*)calloc(1, sizeof(Stack));
	Move *dummyMove;

	if(puzzle->numOfEmptyCells==0) /* there are not any empty cells */
	{
		free(stk);
		return 0;
	}

	init(stk);
	
	do
	{
		if( !success || end) /* invalid cell solution or last cell */
		{
			c = cell->value;
			stk->top->options[c]=0;
			stk->top->options[0]--;
			if (topOption(stk)==0) /* no more options */
			{
				dummyMove=setCell(puzzle, col, row, 0, Edit);
				deleteList(dummyMove);
				pop(stk);
				/*end = true;*/
			}
			else
			{
				c = choice(stk->top->options, puzzle->blockNumOfCells+1); /* next option */
				dummyMove=setCell(puzzle, col, row, c, Edit);
				deleteList(dummyMove);
				end = false;
				success = true;
			}
		}
		else
		{
			do 
			{
				if(col==puzzle->blockNumOfCells && row==puzzle->blockNumOfCells) /* this is the last cell */
				{
					cnt++;
					end = true;
					found = true;
				}
				else
				{
					row = nextCellRow(col, row, puzzle->blockNumOfCells);
					col = nextCellCol(col, puzzle->blockNumOfCells);
					cell = getCell(puzzle, col, row);
					if (cell->value==0)
					{
						found = true;
						push(puzzle, col, row, stk);
						c = choice(stk->top->options, puzzle->blockNumOfCells+1);
						if (c==0)
						{
								pop(stk);
								success = false;
						}
						else
						{
							dummyMove=setCell(puzzle, col, row, c, Edit);
							deleteList(dummyMove);
							success = true;
						}
					}
					else
					{
						found = false;
					}
				}
			} while (!found);
		}

		col = topCol(stk);
		row = topRow(stk);
		cell = getCell(puzzle, col, row);
	} while (!isEmpty(stk));
	
	free(stk);
	return cnt;
}
