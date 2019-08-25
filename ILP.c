#include "ILP.h"

int*** variables;

/* run ILP and return if there is a solution or not */
int ILPSolvable(Puzzle *puzzle)
{
	if (puzzle==NULL)
		return 1;
    return 1;
}

/* run ILP and return a solution */
Puzzle* ILPSolver(Puzzle *puzzle)
{
    return puzzle;
}

/* 
 * run LP and return a new puzzle such that
 * all empty un-fixed cell values with a score
 * of threshold or greater are filled.
 * if several values hold for the same cell the value
 * is randomly chosen according to the score as the probability.
 * fills only legal values
 */
Puzzle* LPSolver(Puzzle *puzzle, float threshold)
{
    if (threshold != 0)
    {
        return puzzle;    
    }
    return puzzle;
}

/* 
 * return a list such that in index i there is the
 * probability that the value of cell is i+1 
 */
float* LPCellValues(Puzzle *puzzle, float threshold, Cell *cell, float *values)
{
    int i, value;
    for (i=0; i<puzzle->blockNumOfCells; i++)
    {
        values[i] = threshold;
    }
    value = cell->value;
    values[value] = 0;

    return values;
}

void addConstrait();

void initVariables(int blockNumOfCells)
{
    int i, j, k, l;

    variables = (int***)calloc(blockNumOfCells, sizeof(int**));
    if (variables == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    
    for (i=0; i<blockNumOfCells; i++)
    {
        variables[i] = (int**)calloc(blockNumOfCells, sizeof(int*));
        if (variables[i] == NULL) /* calloc failed */
        {
            for (j=0; j<i; j++)
            {
                free(variables[j]);
            }
            free(variables);
            printError(MemoryAllocFailed, NULL, 0, 0);
            exit(0);
        }
    }

    for (i=0; i<blockNumOfCells; i++)
    {
        variables[i] = (int**)calloc(blockNumOfCells, sizeof(int*));
        if (variables[i] == NULL) /* calloc failed */
        {
            for (k=0; k<i; k++)
            {
                for (l=0; l<blockNumOfCells; l++)
                {
                    free(variables[k][l]);
                }
                free(variables[k]);
            }
            free(variables);
            printError(MemoryAllocFailed, NULL, 0, 0);
            exit(0);
        }
        else
        {
            for (j=0; j<blockNumOfCells; j++)
            {
                variables[i][j] = (int*)calloc(blockNumOfCells, sizeof(int));
                if (variables[i][j] == NULL) /* calloc failed */
                {
                    for (l=0; l<j; l++)
                    {
                        free(variables[i][l]);
                    }
                    free(variables[i]);
                    for (k=0; k<i; k++)
                    {
                        for (l=0; l<blockNumOfCells; l++)
                        {
                            free(variables[k][l]);
                        }
                        free(variables[k]);
                    }
                    free(variables);
                    printError(MemoryAllocFailed, NULL, 0, 0);
                    exit(0);
                }
            }
        }
    }
}

void freeVariables(int blockNumOfCells)
{
    int i, j, k;
    for (i=0; i<blockNumOfCells; i++)
    {
        for (j=0; j<blockNumOfCells; j++)
        {
            free(variables[i][j]);
        }
        free(variables[i]);
    }
    free(variables);
}

void updateVariables(Puzzle *puzzle, bool integ)
{
    int i, j, k, cnt=1;
    int blockNumOfCells = puzzle->blockNumOfCells;
    int *values, *obj, *vtype;
    values = (int*)calloc(blockNumOfCells+1, sizeof(int));
    if (values == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    obj = (int*)calloc(blockNumOfCells, sizeof(int));
    if (obj == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(values);
        exit(0);
    }
    vtype = (int*)calloc(blockNumOfCells, sizeof(int));
    if (vtype == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(values);
        free(obj);
        exit(0);
    }
    for (i=1; i<puzzle->blockNumOfCells+1; i++)
    {
        for (j=1; j<blockNumOfCells+1; j++)
        {
            values = numOfCellSol(puzzle, i, j, values);
            for (k=1; k<blockNumOfCells+1; k++)
            {
                if (values[k])
                {
                    variables[i-1,j-1,k-1] = cnt;
                    
                    if (integ)
                    {
                        /* obj[cnt-1] = 1;
                        vtype[cnt-1] = GRB_BINARY; */
                    }
                    else
                    {
                        /* obj[cnt-1] = random(); 
                        vtype[cnt-1] = GRB_CONTINUOUS; */
                    }
                    cnt++;
                }
            }
        }
    }
    /* add variables */
    /* change objective sense to maximization */
    free(values);
    free(obj);
    free(vtype);
}

void addCellsConstraints(Puzzle *puzzle)
{
    int i, j, k, cnt=0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    int *ind = (int*)calloc(blockNumOfCells, sizeof(int));
    if (ind == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    int *val = (int*)calloc(blockNumOfCells, sizeof(int));
    if (val == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(ind);
        exit(0);
    }
    for (i=1; i<blockNumOfCells+1; i++)
    {
        for (j=1; j<blockNumOfCells+1; j++)
        {
            for (k=1; k<blockNumOfCells+1; k++)
            {
                if(variables[i-1][j-1][k-1])
                {
                    ind[cnt] = variables[i-1][j-1][k-1]-1;
                    val[cnt] = 1;
                    cnt++;
                }
            }
            /* add constraint cell<i,j> has 1 value */
            cnt=0;
        }
    }
    free(ind);
    free(val);
}

void addRowsConstraints(Puzzle *puzzle)
{
    int i, j, k, cnt=0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    int *ind = (int*)calloc(blockNumOfCells, sizeof(int));
    if (ind == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    int *val = (int*)calloc(blockNumOfCells, sizeof(int));
    if (val == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(ind);
        exit(0);
    }
    for (j=1; j<blockNumOfCells+1; j++)
    {
        for (k=1; k<blockNumOfCells+1; k++)
        {
            for (i=1; i<blockNumOfCells+1; i++)
            {
                if(variables[i-1][j-1][k-1])
                {
                    ind[cnt] = variables[i-1][j-1][k-1]-1;
                    val[cnt] = 1;
                    cnt++;
                }
            }
            /* add constraint row j has 1 k value */
            cnt=0;
        }
    }
    free(ind);
    free(val);
}

void addColsConstraints(Puzzle *puzzle)
{
    int i, j, k, cnt=0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    int *ind = (int*)calloc(blockNumOfCells, sizeof(int));
    if (ind == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    int *val = (int*)calloc(blockNumOfCells, sizeof(int));
    if (val == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(ind);
        exit(0);
    }
    for (i=1; i<blockNumOfCells+1; i++)
    {
        for (k=1; k<blockNumOfCells+1; k++)
        {
            for (j=1; j<blockNumOfCells+1; j++)
            {
                if(variables[i-1][j-1][k-1])
                {
                    ind[cnt] = variables[i-1][j-1][k-1]-1;
                    val[cnt] = 1;
                    cnt++;
                }
            }
            /* add constraint col i has 1 k value */
            cnt=0;
        }
    }
    free(ind);
    free(val);
}

void addBlocksConstraints(Puzzle *puzzle)
{
    int i, j, k, n, m, index, cnt=0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    int blockNumOfCols = puzzle->blockNumCol;
    int blockNumOfRows = puzzle->blockNumRow;
    int *ind = (int*)calloc(blockNumOfCells, sizeof(int));
    if (ind == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    int *val = (int*)calloc(blockNumOfCells, sizeof(int));
    if (val == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(ind);
        exit(0);
    }
    for (n=0; n<blockNumOfRows; n++)
    {
        for (m=0; m<blockNumOfCols; m++)
        {
            for (k=1; k<blockNumOfCells+1; k++)
            {
                for (i=1; i<blockNumOfCols+1; i++)
                {
                    for (j=1; j<blockNumOfRows+1; j++)
                    {
                        index = variables[(n*blockNumOfCols)+(i-1)][(m*blockNumOfRows)+(j-1)][k-1];
                        if(index)
                        {
                            ind[cnt] = index-1;
                            val[cnt] = 1;
                            cnt++;
                        }
                    }
                }
                /* add constraint row j has 1 k value */
                cnt = 0;
            }
        }
    }
    free(ind);
    free(val);
}

void addConstraints(Puzzle *puzzle)
{
    addRowsConstraints(puzzle);
    addColsConstraints(puzzle);
    addBlocksConstraints(puzzle);
}