#include "Solver.h"

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

int nextCellCol(int col, int blockNumOfCells)
{
	return ((col%blockNumOfCells)+1);
}

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

int backtrack(Puzzle *puzzle)
{
	int c, col=0, row=0, cnt=0;
	Cell *cell;
	bool success=true, found=false, end = false;
	Stack *stk = (Stack*)calloc(1, sizeof(Stack));

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
			if (stk->top->options[0]==0) /* no more options */
			{
				setCell(puzzle, col, row, 0, Edit);
				pop(stk);
				/*end = true;*/
			}
			else
			{
				c = choice(stk->top->options, puzzle->blockNumOfCells+1); /* next option */
				setCell(puzzle, col, row, c, Edit);
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
							setCell(puzzle, col, row, c, Edit);
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
