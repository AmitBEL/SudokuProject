#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
#include <stdbool.h>

Puzzle puzzleStrct = {0, 0, 0, 0, 0, 0, 0};
Puzzle *puzzle = &puzzleStrct;

int getNumOfRowInBlock()
{
    return (puzzle->blockNumRow);
}

int getNumOfColInBlock()
{
    return (puzzle->blockNumCol);
}

int getBlockNumOfCells()
{
    return (puzzle->blockNumOfCells);
}

int getNumOfCells()
{
    return (puzzle->numOfCells);
}

/*
 * initialize puzzle fields - free allocated memory
 * pre-condition: puzzle->board!=NULL
 */
void cleanPuzzle()
{
    int i;
    for (i = 0; i < puzzle->blockNumOfCells; i++)
    {
        free(puzzle->board[i]);
    }
    /* ensure that memory was allocated for board before.
     * why we need it:
     * assuming loading a file failed (non-exist file), we don't need to free the board again
     * because we didn't try to allocate memory */
    if (puzzle->blockNumOfCells>0)
    	free(puzzle->board);
    puzzle->blockNumCol = 0;
    puzzle->blockNumRow = 0;
    puzzle->blockNumOfCells = 0;
    puzzle->numOfCells = 0;
    puzzle->numOfEmptyCells = 0;
}

/*
 * 1<=x<=(puzzle->blockNumOfCells)
 * 1<=y<=(puzzle->blockNumOfCells)
 */
void setFillBoard(int x, int y, int newValue) {
	Cell *cell = getCell(puzzle, x, y);
	puzzle->numOfEmptyCells--;
	updateCollisions(puzzle, x, y, newValue);
	cell->value = newValue;
}

bool fillBoard(FILE* fp, Mode mode) {
	int i, j, N=getBlockNumOfCells(), newValue;
	int **board=(int**)calloc(N, sizeof(int*));
	char ch;
	Cell *cell;

	for (i=0;i<N;i++)
		board[i]=(int*)calloc(N, sizeof(int));

	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			cell = getCell(puzzle, j+1, i+1);
			if (fscanf(fp, "%d", &newValue)!=1)
				return false;
			if (!isNumInRange(newValue, 0, N))
				return false;
			/*printf("%d\n", newValue);*/
			if (mode==Solve)
				board[i][j]=newValue;
			if (newValue!=0) {
				if (mode==Solve)
				{
					if(fscanf(fp, "%c", &ch)==1){
						if (ch=='.') /* cell is not fixed when loading in edit mode */
						{
							setFillBoard(j+1, i+1, newValue);
							cell->fixed = 1;
						}
					}
				}
				else /* mode==Edit */
				{
					setFillBoard(j+1, i+1, newValue);
					fscanf(fp, "%c", &ch);
				}

			}
		}
	}



	if (mode==Solve)
	{
		/* solve C:\\sudoku\\in8.txt should fail */
		if (isErroneous())
			return false;

		for(i=0;i<N;i++){
			for(j=0;j<N;j++){
				setFillBoard(j+1, i+1, board[i][j]);
			}
		}
	}

	return true;
}

/*
 * return true if assignment to board succeeded,
 * otherwise return false (failure in this function is treated as function failed, parameter 5 in parser.c)
 * notes:
 * https://moodle.tau.ac.il/mod/forum/discuss.php?d=88124
 */
bool load(char* filepath, Mode mode) {
	FILE* fp;
	int m,n;

	fp = fopen(filepath, "r");
	if (fp == NULL) {
		printError(ReadingFileFailed, NULL, 0, 0);
		return false;
	}

	if (fscanf(fp, "%d %d", &m, &n)!=2) {
		printError(ReadingFileFailed, NULL, 0, 0);
		fclose(fp);
		return false;
	}

	if (m*n>99){
		printError(BigBoard, NULL,0,0);
		fclose(fp);
		return false;
	}

	if (m<=0 || n<=0)
	{
		printError(DimNotPositive, NULL, 0, 0);
		fclose(fp);
		return false;
	}

	createBoard(m, n);
	if (!fillBoard(fp, mode)) {
		printBoard(1);
		if (puzzle->board!=NULL)
			cleanPuzzle();
		printError(IllegalBoard, NULL, 0, 0);
		fclose(fp);
		return false;
	}

	fclose(fp);

	return true;
}

/*
 * notes:
 * https://moodle.tau.ac.il/mod/forum/discuss.php?d=87711
 */
bool save(char* filepath, Mode mode) {
	FILE *fp = fopen(filepath, "w");
	int N=puzzle->blockNumOfCells, i, j;
	Cell *cell;

	if (fp == NULL){
		fclose(fp);
		return false;
	}
	fprintf(fp, "%d %d\n", puzzle->blockNumRow, puzzle->blockNumCol);
	for(i=0;i<N;i++){
			for(j=0;j<N;j++){
				cell = getCell(puzzle, j+1,i+1);
				if (j>0){
					fprintf(fp, " ");
				}
				fprintf(fp, "%d", cell->value);
				if (cell->fixed==1 || (mode==Edit && cell->value!=0)){
					fprintf(fp, ".");
				}
			}
			fprintf(fp, "\n");
	}

	if (fclose(fp)!=0)
		return false;

	return true;
}

/* load puzzle for solve mode */
bool solve(char *filepath, Mode mode)
{
    if (puzzle->board != NULL)
    {
        cleanPuzzle();
    }

    return load(filepath, mode);
}

/* create new empty puzzle board */
void createBoard(int blockNumOfRows, int blockNumOfCols)
{
	int i, j;

    puzzle->blockNumRow = blockNumOfRows;
    puzzle->blockNumCol = blockNumOfCols;
    puzzle->blockNumOfCells = blockNumOfCols * blockNumOfRows;
    puzzle->numOfCells = puzzle->blockNumOfCells * puzzle->blockNumOfCells;
    puzzle->numOfEmptyCells = puzzle->numOfCells;
    puzzle->numOfErroneous = 0;

    puzzle->board = (Cell **)calloc(puzzle->blockNumOfCells, sizeof(Cell *));
    if (puzzle->board == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    for (i = 0; i < puzzle->blockNumOfCells; i++)
    {
        puzzle->board[i] = (Cell *)calloc(puzzle->blockNumOfCells, sizeof(Cell));
        if (puzzle->board[i] == NULL) /* calloc failed */
        {
            for (j = 0; j < i; j++)
            {
                free(puzzle->board[j]);
            }
            free(puzzle->board);
            printError(MemoryAllocFailed, NULL, 0, 0);
            exit(0);
        }
    }
}

/* create new empty 9x9 puzzle */
bool editNew()
{
    if (puzzle->board != NULL)
    {
        cleanPuzzle();
    }
    createBoard(3, 3);
    return true;
}

/* load puzzle for edit mode */
bool editFile(char *filepath, Mode mode)
{
    if (puzzle->board != NULL)
    {
        cleanPuzzle();
    }
    return (load(filepath, mode));
}

/* 
 * print puzzle board
 * if mark=1 it marks the erroneous cells
 */
void printBoard(int mark)
{
    Cell *cell;
    int cellRow, cellCol;
    int i, j, k, r, l;
    int M = puzzle->blockNumRow; /* num of blocks in a row */
    int N = puzzle->blockNumCol; /* num of cells in a block row */
    int C = 4;           /* num of chars every cell take to print */
    int numOfdashes = M * (N * C + 1) + 1;

    printf("errors: %d\n", puzzle->numOfErroneous);

    for (l = 0; l < numOfdashes; l++) /* print first dashes row */
    {
        printf("-");
    }
    printf("\n");

    for (k = 0; k < N; k++) /* for every row of blocks */
    {
        for (r = 0; r < M; r++) /* for every row of a block */
        {
            for (j = 0; j < M; j++) /* for every col of blocks */
            {
                printf("|");
                for (i = 0; i < N; i++) /* for every col of a block */
                {
                    cellRow = (k * M) + r + 1;
                    cellCol = (j * N) + i + 1;
                    cell = getCell(puzzle, cellCol, cellRow);
                    if (cell->value == 0) /* empty cell */
                    {
                        printf("   ");
                    }
                    else
                    {
                        printf(" %2d", cell->value);
                    }
                    if (cell->fixed) /* fixed cell */
                    {
                        printf(".");
                    }
                    else
                    {
                        if (mark && cell->numOfCollisions) /* erroneous cell */
                        {
                            printf("*");
                        }
                        else
                        {
                            printf(" ");
                        }
                    }
                }
            }
            printf("|\n");
        }
		for (l = 0; l < numOfdashes; l++)
		{
			printf("-");
		}
		printf("\n");
    }
    return;
}

/*
 * call setCell
 */
Move* set(int x, int y, int z, Mode mode)
{
    return setCell(puzzle, x, y, z, mode);
}

bool isSolved()
{
	return (!(puzzle->numOfEmptyCells) && !isErroneous() ? true : false);
}

bool isErroneous() {
	return (puzzle->numOfErroneous > 0 ? true : false);
}

/* check if the board is solvable */
bool validate(bool printResult)
{
    if (ILPSolvable(puzzle))
    {
    	if (printResult)
    		printf("Validation passed: board is solvable\n");
        return true;
    }
    else
    {
    	if (printResult)
    		printf("Validation failed: board is unsolvable\n");
        return false;
    }
}

/* 
 * fill empty cells using LP
 * fill only legal values with score greater than threshold
 */
Move* guess(float threshold, Mode mode)
{
    Move *head = NULL;
    Puzzle *LPSolution;
    Cell *cell;
    Move* m;
    int i, j;
    
    LPSolution = LPSolver(puzzle, threshold);
    for (i = 0; i < puzzle->blockNumOfCells; i++)
    {
        for (j = 0; j < puzzle->blockNumOfCells; j++)
        {
            cell = getCell(puzzle, i + 1, j + 1);
            if (!(cell->fixed))
            {
            	m=set(i + 1, j + 1, LPSolution->board[j][i].value, mode);
                concat(&head, &m);
            }
        }
    }
    return head;
}

void hint(int x, int y)
{
    Puzzle *ILPSolution;
    Cell *cell;
    int value;
    cell = getCell(puzzle, x, y);
    if (cell->fixed)
    {
        printf("Error: cell <%d,%d> is fixed\n", x, y);
        return;
    }
    if (cell->value)
    {
        printf("Error: cell <%d,%d> already contains a value\n", x, y);
    }
    else
    {
        ILPSolution = ILPSolver(puzzle);
        value = ILPSolution->board[y - 1][x - 1].value;
        printf("Hint: set cell <%d,%d> to %d\n", x, y, value);
    }
    return;
}

void guessHint(int x, int y)
{
    Cell *cell;
    float *values;
    int i;
    cell = getCell(puzzle, x, y);
    if (cell->fixed)
    {
        printf("Error: cell <%d,%d> is fixed\n", x, y);
        return;
    }
    if (cell->value)
    {
        printf("Error: cell <%d,%d> already contains a value\n", x, y);
    }
    else
    {
        values = (float *)calloc(puzzle->blockNumOfCells, sizeof(float));
        values = LPCellValues(puzzle, 0, cell, values);
        printf("Hint: cell <%d,%d> legal values are\n", x, y);
        for (i = 0; i < puzzle->blockNumOfCells; i++)
        {
            if (values[i] > 0)
            {
                printf("%d with score %f\n", i + 1, values[i]);
            }
        }
        free(values);
    }
    return;
}

void numSolution()
{
    int solNum;
    solNum = backtrack(puzzle);
    printf("The number of solutions is %d", solNum);
    return;
}

int numOfEmptyCells() {
	return puzzle->numOfEmptyCells;
}

void printCustomBoard(Cell** board, int limit1, int limit2)
{
	int i,j;
	printf("\nfill (%d,%d):\n", limit1, limit2);
	for (i=0; i<limit1; i++)
	{
		for (j=0; j<limit2; j++)
		{
			printf("%d ", board[i][j].value);
		}
		printf("\n");
	}
	printf("\n");
}

Move* autoFill(Mode mode)
{
    Move *head = NULL;
    int i, j, k, value;
    Cell *cell;
    int *values = (int *)calloc((puzzle->blockNumOfCells) + 1, sizeof(int));
    Puzzle newPuzzle = {0, 0, 0, 0, 0, 0, 0};
    Puzzle *toFill = &newPuzzle; /* create new empty puzzle */
    Move* m;

    toFill->blockNumCol = puzzle->blockNumCol;
    toFill->blockNumOfCells = puzzle->blockNumOfCells;
    toFill->blockNumRow = puzzle->blockNumRow;
    toFill->numOfCells = puzzle->numOfCells;
    toFill->numOfEmptyCells = puzzle->numOfEmptyCells;
    toFill->numOfErroneous = puzzle->numOfErroneous;
    toFill->board = (Cell **)calloc(puzzle->blockNumOfCells, sizeof(Cell *)); /* create empty board to the new puzzle */
    printf("A");
    if (toFill->board == NULL)
    { /* calloc failed */
        printError(MemoryAllocFailed, NULL,0,0);
    }
    for (i = 0; i < toFill->blockNumOfCells; i++)
    {
        toFill->board[i] = (Cell *)calloc(puzzle->blockNumOfCells, sizeof(Cell));
        if (toFill->board[i] == NULL)
        { /* calloc failed */
            for (j = 0; j < i; j++)
            {
                free(toFill->board[j]);
            }
            free(toFill->board);
            printError(MemoryAllocFailed, NULL,0,0);
        }
    }
    printf("B");
    for (i = 0; i < puzzle->blockNumOfCells; i++)
    { /* fill the new puzzle board with obvious values only */
        for (j = 0; j < puzzle->blockNumOfCells; j++)
        {
            cell = getCell(puzzle, j+1, i+1);
            printf("C");
            if (!(cell->value))
            {
            	printf("D");
                values = numOfCellSol(puzzle, j+1, i+1, values);
                printf("cell <%d,%d> values[0]=%d\n", j+1, i+1, values[0]);
                if (values[0] == 1)
                {
                    for (k = 1; k < (puzzle->blockNumOfCells)+1; k++)
                    {
                        printf("k=%d", k);
                    	printf("values[%d]=%d, ", k, values[k]);
                        printCustomBoard(toFill->board, puzzle->blockNumOfCells, puzzle->blockNumOfCells);
                    	if (values[k] == 1)
                        {
                            /*value = k;*/
                    		(toFill->board[i][j]).value = k;
                    		printf("....\n");
                    		printCustomBoard(toFill->board, puzzle->blockNumOfCells, puzzle->blockNumOfCells);
                            break;
                        }
                    }
                    /*(toFill->board[i][j]).value = value;*/
                }
            }
        }
    }
    printf("E");
    for (i = 0; i < puzzle->blockNumOfCells; i++)
    { /* fill the original puzzle board with the obvious values from the new puzzle board */
        for (j = 0; j < puzzle->blockNumOfCells; j++)
        {
            cell = getCell(puzzle, j+1, i+1);
            if (!(cell->value))
            {
                value = toFill->board[i][j].value;
                if (value)
                {
                	m=set(j + 1, i + 1, value, mode);
                	printf("<%d,%d> after set", j+1, i+1);
                    concat(&head, &m);
                    printf(" after concat\n");
                }
            }
        }
    }

    for (i = 0; i < toFill->blockNumOfCells; i++)
    { /* free allocated memory */
        free(toFill->board[i]);
    }
    free(toFill->board);
    free(values);

    return head;
}

void Exit()
{
	if (puzzle->board!=NULL)
		cleanPuzzle();
    printf("Exiting...\n");
    exit(0);
}
