#include "Generator.h"

/*
 * 1. this module always use setCell to keep numOfEmptyCells and numOfErroneous up to date
 * 2. delete the move that setCell returns to prevent memory leak (maybe not needed)
 */

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
	int *values = (int*)calloc(puzzle->blockNumOfCells+1, sizeof(int));
	int randValue, i, counter=0;
	Move *dummyMove;

	printf("legal values for <%d,%d>\n", col+1, row+1);

	values=numOfCellSol(puzzle, col, row, values);

	for (i=0;i<puzzle->blockNumOfCells+1;i++)
	{
		printf("%d ", i);
	}
	printf("\n");
	for (i=0;i<puzzle->blockNumOfCells+1;i++)
	{
		printf("%d ", values[i]);
	}
	printf("\n");

	if (values[0]>0)
	{
		randValue=(rand()%(values[0])); /* choose random index from indexes of legal values */
		for (i=1;i<puzzle->blockNumOfCells+1;i++)
		{
			if (values[i]!=0)
			{
				if (counter==randValue)
				{
					dummyMove=setCell(puzzle, col+1, row+1, i, Edit);
					deleteList(dummyMove);
					return;
				}
				counter++;
			}
		}
	}
}

/*
 * generate puzzle from given puzzle
 */
Move* generate(Puzzle *puzzle, int x, int y){
	int i=0, j=0, N=puzzle->blockNumOfCells, col, row, r, s;
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

	for (i=0;i<N;i++)
	{
		backupBoard[i]=(int*)calloc(N, sizeof(int));
		if (backupBoard[i]==NULL)
		{
			printError(MemoryAllocFailed, NULL, 0, 0);
			exit(0);
		}
	}

	copyBoard(puzzle, backupBoard);

	for (r=0; r<puzzle->blockNumOfCells; r++)
	{
		for (s=0; s<puzzle->blockNumOfCells; s++)
		{
			printf("%d ", backupBoard[r][s]);
		}
		printf("\n");
	}
	printf("\n");

	/* try 1000 times to generate a board */
	for (i=0;i<1000;i++)
	{
		printf("\n\n\n\n\n\n\n\ntry #%d:\n\n\n", i);
		printCustomBoard(puzzle->board, puzzle->blockNumOfCells, puzzle->blockNumOfCells);
		/* choose x empty cells randomly and fill them with legal values */
		j=0;
		while (j<x)
		{
			col=rand()%N;
			row=rand()%N;
			printf("j=%d, col=%d, row=%d\n", j, col, row);
			cell=getCell(puzzle, col+1, row+1);
			if (cell->value==0)
			{
				print("before fillRandPossibleValue\n");
				fillRandPossibleValue(puzzle, col, row);
				print("after fillRandPossibleValue\n");
				if (cell->value==0) /* legal value not exists */
				{
					/*changeBoard(puzzle, backupBoard);  init the puzzle to backupBoard */
					xEmptyCellsFilled=false;
					break;
				}
				j++;
			}
		}

		printCustomBoard(puzzle->board, puzzle->blockNumOfCells, puzzle->blockNumOfCells);

		if (xEmptyCellsFilled)
		{
			printf("\n\nfound %d empty cells\n\n", x);
			/* solve the board with ILP */
			puzzle = ILPSolver(puzzle);
		}

		printCustomBoard(puzzle->board, puzzle->blockNumOfCells, puzzle->blockNumOfCells);

		/* if x empty cells not filled or there is no solution, at least 1 empty cell exists */
		if (puzzle->numOfEmptyCells==0/* && xEmptyCellsFilled*/)
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
			printf("only %d cells with values", y);
			printCustomBoard(puzzle->board, puzzle->blockNumOfCells, puzzle->blockNumOfCells);
			generateSucceeded=true;
		}
		else
		{
			changePuzzle(puzzle, backupBoard); /* init the puzzle to backupBoard */
			continue;
		}
	}

	print("after all iterations");
	/* save the cells that changed in moves list */
	if (generateSucceeded)
	{
		print("succeeded");
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

	return moves;
}
