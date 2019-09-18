#include "Generator.h"

/*
 * @pre-condition: puzzle->blockNumOfCells = (source_width) = (source_height)
 * changes the puzzle state to source state
 */
void changePuzzle(Puzzle* puzzle, int** source)
{
	int i,j;
	Move *dummyMove;

	for (i=0;i<puzzle->blockNumOfCells;i++)
	{
		for (j=0;j<puzzle->blockNumOfCells;j++)
		{
			dummyMove=setCell(puzzle, j+1, i+1, source[i][j], Edit);
			deleteList(dummyMove);
		}
	}
}

/*
 * @pre-condition: puzzle->blockNumOfCells = (dest_width) = (dest_height)
 * changes the dest state to source->board
 */
void copyBoard(Puzzle* source, int** dest)
{
	int i,j;
	Cell* cell;

	for (i=0;i<source->blockNumOfCells;i++)
	{
		for (j=0;j<source->blockNumOfCells;j++)
		{
			cell=getCell(source, j+1, i+1);
			dest[i][j]=cell->value;
		}
	}
}

/*
 * fill puzzle->board[row][col] with random legal value
 */
void fillRandPossibleValue(Puzzle *puzzle, int col, int row)
{
	int randValue, i, counter=0;
	Move *dummyMove;
	int *values = (int*)calloc(puzzle->blockNumOfCells+1, sizeof(int));

	if (values==NULL)
	{
		printError(MemoryAllocFailed, NULL, 0, 0);
		exit(0);
	}


	values=numOfCellSol(puzzle, col, row, values);

	if (values[0]>0)
	{
		randValue=(rand()%(values[0])); /* choose random index from indexes of legal values */
		for (i=1;i<puzzle->blockNumOfCells+1;i++)
		{
			if (values[i]!=0)
			{
				if (counter==randValue)
				{
					dummyMove=setCell(puzzle, col, row, i, Edit);
					deleteList(dummyMove);
					free(values);
					return;
				}
				counter++;
			}
		}
	}
	free(values);
}

/*
 * generates a puzzle by randomly filling x empty cells
 * with legal values, running ILP to solve the board,
 * and then clearing all but y random cells
 * return list of cells that changed
 */
Move* generate(Puzzle *puzzle, int x, int y){
	int i=0, j=0, N=puzzle->blockNumOfCells, col, row/*, r, s*/;
	Cell *cell;
	Move *moves=NULL, *dummyMove;
	bool generateSucceeded=false, xEmptyCellsFilled=true;
	int **backupBoard=(int**)calloc(N, sizeof(int*));

	/* backup current puzzle->board */
	if (backupBoard==NULL)
	{
		printError(MemoryAllocFailed, NULL, 0, 0);
		exit(0);
	}


	if (isBoardErr(puzzle))
	{
		if(moves==NULL)
		{
			addToList(&moves, 0, 0, 0, 0);
		}
		free(backupBoard);
		return moves;
	}


	for (i=0;i<N;i++)
	{
		backupBoard[i]=(int*)calloc(N, sizeof(int));
		if (backupBoard[i]==NULL)
		{
			for (j = 0; j < i; j++)
			{
				free(backupBoard[j]);
			}
			free(backupBoard);
			printError(MemoryAllocFailed, NULL, 0, 0);
			exit(0);
		}
	}

	copyBoard(puzzle, backupBoard);

	/* try 1000 times to generate a board */
	for (i=0;i<1000;i++)
	{

		/* choose x empty cells randomly and fill them with legal values */
		j=0;
		xEmptyCellsFilled=true;

		while (j<x)
		{
			col=(rand()%N)+1;
			row=(rand()%N)+1;

			cell=getCell(puzzle, col, row);
			if (cell->value==0)
			{
				fillRandPossibleValue(puzzle, col, row);

				if (cell->value==0) /* legal value not exists */
				{

					xEmptyCellsFilled=false;
					break;
				}
				j++;
			}
		}



		if (xEmptyCellsFilled)
		{

			/* solve the board with ILP */
			puzzle = ILPSolver(puzzle);

		}



		/* if x empty cells not filled or there is no solution, at least 1 empty cell exists */
		if (puzzle->numOfEmptyCells==0)
		{
			/* clear all values except y chosen values */
			j=0;
			while(j < ((puzzle->numOfCells) - y))
			{
				col=rand()%N;
				row=rand()%N;
				cell=getCell(puzzle, col+1, row+1);
				if (cell->value!=0)
				{
					dummyMove=setCell(puzzle, col+1, row+1, 0, Edit);
					deleteList(dummyMove);
					j++;
				}
			}
			generateSucceeded=true;
			break;
		}
		else
		{
			changePuzzle(puzzle, backupBoard); /* init the puzzle to backupBoard */
		}
	}


	/* save the cells that changed in moves list */
	if (generateSucceeded)
	{

		for (i=0;i<puzzle->blockNumOfCells;i++)
		{
			for (j=0;j<puzzle->blockNumOfCells;j++)
			{
				cell=getCell(puzzle, j+1, i+1);
				if (cell->value!=backupBoard[i][j])
				{
					addToList(&moves, j+1, i+1, backupBoard[i][j], cell->value);
				}
			}
		}
	}


	if(moves==NULL)
	{
		addToList(&moves, 0, 0, 0, 0);
	}

	for (i=0; i<N; i++)
	{
		free(backupBoard[i]);
	}
	free(backupBoard);

	return moves;
}
