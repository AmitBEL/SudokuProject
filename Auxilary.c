#include "Auxilary.h"


/*
 * Check whether numOfFixed is between minNum and maxNum
 * Return true if it is
 */
bool isNumInRange(int num, int minNum, int maxNum) {
	return (minNum <= num && num <= maxNum);
}

/*
 * return the first row in the block
 * @pre: row is 0-based
 */
int firstRowInBlock(int row, int blockNumRows)
{
	return (row / blockNumRows) * blockNumRows;
}

/*
 * return the first col in the block
 * @pre: col is 0-based
 */
int firstColInBlock(int col, int blockNumCols)
{
	return (col / blockNumCols) * blockNumCols;
}

/* return a pointer to cell <x,y> */
Cell* getCell(Puzzle *puzzle, int x, int y)
{
    int row = y - 1;
    int col = x - 1;
    return (&(puzzle->board[row][col]));
}

/*
 * set cell <x,y> value to z
 * update collisions with the previous value and the new value
 * update the number of empty cells
 * if all the cells are not empty and game mode is solve
 * check if the puzzle can be solved
 * print a message and update game mode according to the check
 *  */
Move* setCell(Puzzle *puzzle, int x, int y, int z, Mode mode)
{
    Move *head = NULL;
    Cell *cell;
    cell = getCell(puzzle, x, y);
    if (mode == Solve && cell->fixed)
    {
        printf("Error: cell is fixed\n");
        return head;
    }
    if (cell->value==z)
    {
        return head;
    }
    if (cell->value == 0)
    {
        if (z != 0)
        {
            puzzle->numOfEmptyCells--;
        }
    }
    else
    {
        if (z == 0)
        {
            puzzle->numOfEmptyCells++;
        }
    }
    updateCollisions(puzzle, x, y, z);
    addToList(&head, x, y, cell->value, z);
    cell->value = z;
    return head;
}

/* update collisions of the new and old values */
void updateCollisions(Puzzle *puzzle, int x, int y, int newValue)
{
    Cell* cell=getCell(puzzle, x,y);
    if (cell->numOfCollisions)
    {
        puzzle->numOfErroneous--;
    }
    cell->numOfCollisions=0;
    updateRowCollisions(puzzle, x, y, newValue);
    updateColCollisions(puzzle, x, y, newValue);
    updateBlockCollisions(puzzle, x, y, newValue);
}

/* values[0] = num of legal values
 * values[i] = 1 if i is legal value for cell <x,y> and 0 otherwise
 * if cell <x,y> value is v!=0 return values[0]=values[v]=1 and 0 anywhere else
 * does not assume values is initialized
 */
int *numOfCellSol(Puzzle *puzzle, int x, int y, int *values)
{
    int i, j, value, firstCol, firstRow;
    Cell *cell, *colCell;

    cell = getCell(puzzle, x, y);
    if (cell->value) /* cell is not empty */
    {
        values[0]=1;
        for (i=0; i<puzzle->blockNumOfCells; i++)
        {
            if (cell->value==i+1)
            {
                values[i+1]=1;
            }
            else
            {
                values[i+1]=0;
            }

        }
        return values;
    }

    for (i=0; i<puzzle->blockNumOfCells; i++)
    {
        values[i+1] = 1;
    }

    /* update values according to col */
    for (i=0; i<puzzle->blockNumOfCells; i++)
    {
        cell = getCell(puzzle, x, i+1);
        value = cell->value;
        values[value] = 0;
    }

    /* update values according to row */
    for (i=0; i<puzzle->blockNumOfCells; i++)
    {
        cell = getCell(puzzle, i+1, y);
        value = cell->value;
        values[value] = 0;
    }

    /* update values according to block */
    firstCol = firstColInBlock(x-1, puzzle->blockNumCol);
    firstRow = firstRowInBlock(y-1, puzzle->blockNumRow);
    for (i = firstRow; i < firstRow + puzzle->blockNumRow; i++)
	{
		for (j = firstCol; j < firstCol + puzzle->blockNumCol; j++)
		{
			colCell = getCell(puzzle, j+1, i+1);
            value = colCell->value;
            values[value] = 0;
		}
	}

    /* update values[0] according to the number of valid cell sol */
    value = 0;
    for (i=0; i<puzzle->blockNumOfCells; i++)
    {
        if (values[i+1])
        {
            value++;
        }
    }
    values[0] = value;

    return values;
}

/*
 * substruct cell num of collision by 1
 * and update puzzle's num of erroneous accordingly
 */
void subCollision(Puzzle *puzzle, Cell *cell)
{
	((cell->numOfCollisions)--);
    if (!(cell->numOfCollisions))
    {
        ((puzzle->numOfErroneous)--);
    }
}

/*
 * add 1 to cell num of collision
 * and update puzzle's num of erroneous accordingly
 */
void addCollision(Puzzle *puzzle, Cell *cell)
{
    if (!(cell->numOfCollisions))
    {
        ((puzzle->numOfErroneous)++);
    }
    ((cell->numOfCollisions)++);
}

/* update all the collision of cell <x,y> row */
void updateRowCollisions(Puzzle *puzzle, int x, int y, int newValue)
{
    int i;
    Cell *cell = getCell(puzzle, x, y);
    Cell *colCell;
    int oldValue = cell->value;
    /*if (oldValue==newValue)
    {
        return;
    }
    cell->numOfCollisions = 0; *//* initialize cell <x,y> num of collisions */
    for (i=0; i<puzzle->blockNumOfCells; i++)
    {
    	if (i+1 == x)
    	{
    		continue;
    	}
        colCell = getCell(puzzle, i+1,y);
        if ((colCell->value == oldValue) && (oldValue != 0)) /* collision with the old value */
        {
            subCollision(puzzle, colCell);
        }
        if ((colCell->value == newValue) && (newValue != 0)) /* collision with the new value */
        {
            addCollision(puzzle, cell);
            addCollision(puzzle, colCell);
        }
    }
}

/* update all the collision of cell <x,y> col */
void updateColCollisions(Puzzle *puzzle, int x, int y, int newValue)
{
    int i;
    Cell *cell = getCell(puzzle, x, y);
    Cell *colCell;
    int oldValue = cell->value;
    /*if (oldValue==newValue)
    {
        return;
    }
    cell->numOfCollisions = 0;*/ /* initialize cell <x,y> num of collisions */
    for (i=0; i<puzzle->blockNumOfCells; i++)
    {
    	if (i+1 == y)
    	{
    		continue;
    	}
        colCell = getCell(puzzle, x,i+1);
        if ((colCell->value == oldValue) && (oldValue != 0)) /* collision with the old value */
        {
            subCollision(puzzle, colCell);
        }
        if ((colCell->value == newValue) && (newValue != 0)) /* collision with the new value */
        {
            addCollision(puzzle, cell);
            addCollision(puzzle, colCell);
        }
    }
}

/* update all the collision of cell <x,y> block */
void updateBlockCollisions(Puzzle *puzzle, int x, int y, int newValue)
{
    int i, j, firstRow, firstCol;
    Cell *cell = getCell(puzzle, x, y);
    Cell *colCell;
    int oldValue = cell->value;
    /*if (oldValue==newValue)
    {
        return;
    }
    cell->numOfCollisions = 0;*/ /* initialize cell <x,y> num of collisions */

    firstRow = firstRowInBlock(y-1, puzzle->blockNumRow); /* index of first row in the block */
	firstCol = firstColInBlock(x-1, puzzle->blockNumCol); /* index of first column in the block */

    for (i = firstRow; i < firstRow + puzzle->blockNumRow; i++)
	{
    	if (i+1==y) /* do not check the same row twice */
    		continue;
		for (j = firstCol; j < firstCol + puzzle->blockNumCol; j++)
		{
			if (j+1==x) /* do not check the same column twice */
				continue;

			colCell = getCell(puzzle, j+1, i+1);
            if ((colCell->value == oldValue) && (oldValue != 0)) /* collision with the old value */
            {
                subCollision(puzzle, colCell);
            }
            if ((colCell->value == newValue) && (newValue != 0)) /* collision with the new value */
            {
                addCollision(puzzle, cell);
                addCollision(puzzle, colCell);
            }
		}
	}
}
