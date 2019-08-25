#include "ILP.h"

/* private functions declaration */
void initVariables(int blockNumOfCells);

void freeVariables(int blockNumOfCells);

int updateVariables(Puzzle *puzzle, bool integer/*, GRBmodel model*/);

void addCellsConstraints(Puzzle *puzzle/*, GRBmodel model*/);

void addRowsConstraints(Puzzle *puzzle/*, GRBmodel model*/);

void addColsConstraints(Puzzle *puzzle/*, GRBmodel model*/);

void addBlocksConstraints(Puzzle *puzzle/*, GRBmodel model*/);

void addConstraints(Puzzle *puzzle/*, GRBmodel model*/);

bool findSolution(Puzzle *puzzle, bool integer, int *numOfVariables, int *sol);

void fillIntSolution(Puzzle *puzzle, int *sol, Mode mode);

Move* fillCellAccordingToProb(Puzzle *puzzle, int x, int y, float *values);

Move* fillThresholdSolution(Puzzle *puzzle, int *sol, float threshold, Mode mode);



int*** variables;

/* run ILP and return if there is a solution or not */
int ILPSolvable(Puzzle *puzzle)
{
	int numOfVariables;
    int *sol;
    bool success;
    success = findSolution(puzzle, true, &numOfVariables, sol);
    free(sol);
    freeVariables(puzzle->blockNumOfCells);
    return success;
}

/* run ILP and return cell <x,y> value */
int ILPCellSolver(Puzzle *puzzle, int x, int y)
{
    int numOfVariables, value=0, index, k;
    int *sol;
    bool success;
    success = findSolution(puzzle, true, &numOfVariables, sol);
    if (success)
    {
        for (k=1; k<puzzle->blockNumOfCells+1; k++)
        {
            index = variables[x-1][y-1][k-1];
            if (index)
            {
                value = k;
            }
        }
    }
    free(sol);
    freeVariables(puzzle->blockNumOfCells);
    return value;
}

/* 
 * run LP and return a new puzzle such that
 * all empty un-fixed cell values with a score
 * of threshold or greater are filled.
 * if several values hold for the same cell the value
 * is randomly chosen according to the score as the probability.
 * fills only legal values
 */
Move* LPSolver(Puzzle *puzzle, float threshold, Mode mode)
{
    Move *head = NULL;
    int numOfVariables, value=0, index, k;
    int *sol;
    bool success;
    success = findSolution(puzzle, false, &numOfVariables, sol);
    if (success)
    {
        head = fillThresholdSolution(puzzle, sol, threshold, mode);
    }
    free(sol);
    freeVariables(puzzle->blockNumOfCells);
    return head;
}

/* run ILP and fill puzzle with the solution */
Puzzle* ILPSolver(Puzzle *puzzle)
{
    int numOfVariables, value=0, index, k;
    int *sol;
    bool success;
    success = findSolution(puzzle, true, &numOfVariables, sol);
    if (success)
    {
        fillIntSolution(puzzle, sol, Edit);
    }
    free(sol);
    freeVariables(puzzle->blockNumOfCells);
    return puzzle;
}

/* 
 * return a list such that in index i there is the
 * probability that the value of cell is i+1 
 */
float* LPCellValues(Puzzle *puzzle, float threshold, int x, int y, float *values)
{
    int numOfVariables, cellSol, index, k;
    int *sol;
    bool success;
    success = findSolution(puzzle, false, &numOfVariables, sol);
    if (success)
    {
        for (k=1; k<puzzle->blockNumOfCells+1; k++)
        {
            index = variables[x-1][y-1][k-1];
            if (index)
            {
                cellSol = sol[index-1];
                if (cellSol>=threshold)
                {
                    values[k-1] = cellSol;
                }
                else
                {
                    values[k-1] = 0;
                }
            }
            else
            {
                values[k-1] = 0;
            }
        }
    }
    free(sol);
    freeVariables(puzzle->blockNumOfCells);
    return values;
}

/* use only in Solve mode for guess */
Move* fillCellAccordingToProb(Puzzle *puzzle, int x, int y, float *valueVsScore)
{
	float sum=0, randVal=((rand()%10000)/10000.0);
	int i=0, N=puzzle->blockNumOfCells;

	/* sum the scores */
	for (i=0;i<N;i++)
		sum+=valueVsScore[i];
	if (sum==0)
		return NULL;

	/* change the score of each cell to its relative probability */
	for (i=0;i<N;i++)
			valueVsScore[i]=(valueVsScore[i]/sum);

	/* choose value randomly */
	i=0;
	while (i<N && sum<=randVal)
	{
		sum+=valueVsScore[i];
		i++;
	}

	return setCell(puzzle, x, y, i, Solve); /* the returned value is random value */
}

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

int updateVariables(Puzzle *puzzle, bool integer/*, GRBmodel model*/)
{
    int i, j, k, cnt=1;
    int blockNumOfCells = puzzle->blockNumOfCells;
    int *values, *obj, *vtype;
    Cell *cell;
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
            cell = getCell(puzzle, i, j);
            if (!(cell->value))
            {
                values = numOfCellSol(puzzle, i, j, values);
                for (k=1; k<blockNumOfCells+1; k++)
                {
                    if (values[k])
                    {
                        variables[i-1][j-1][k-1] = cnt;
                        
                        if (integer)
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
    }
    if (integer)
    {
        /* add variables according to GRB_BINARY */
    }
    else
    {
        /* add variables according to GRB_CONTINUOUS */
    }
    /* change objective sense to maximization */
    free(values);
    free(obj);
    free(vtype);
    return (cnt-1);
}

void addCellsConstraints(Puzzle *puzzle/*, GRBmodel model*/)
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

void addRowsConstraints(Puzzle *puzzle/*, GRBmodel model*/)
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

void addColsConstraints(Puzzle *puzzle/*, GRBmodel model*/)
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

void addBlocksConstraints(Puzzle *puzzle/*, GRBmodel model*/)
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

void addConstraints(Puzzle *puzzle/*, GRBmodel model*/)
{
    addCellsConstraints(puzzle/*, model*/);
    addRowsConstraints(puzzle/*, model*/);
    addColsConstraints(puzzle/*, model*/);
    addBlocksConstraints(puzzle/*, model*/);
}

bool findSolution(Puzzle *puzzle, bool integer, int *numOfVariables, int *sol)
{
    /*int numOfVariables;*/
    /* GRBenv *env = NULL; 
    GRBmodel *model = NULL; */ 
    int optimstatus;
    int blockNumOfCells = puzzle->blockNumOfCells;
    double objval;
    bool success;
    /*int *sol*/ 
    if (sol == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    createEnvironment(/*&env, */"logFileName.log");
    createModel(/*env, &model, */"modelName");
    initVariables(puzzle->blockNumOfCells);
    *numOfVariables = updateVariables(puzzle, integer/*, model*/);
    updateModel(/*model*/);
    addConstraints(puzzle/*, model*/);
    optimize(/*model*/);
    write(/*model, */"lpFileName.lp");
    getintattr(/*model ,*/&optimstatus);
    getdblattr(/*model, */&objval);

    sol = (int*)calloc(*numOfVariables, sizeof(int)); 
    getdblattrarray(/*model, */*numOfVariables, sol);

    if (optimstatus == 1/*GRB_OPTIMAL*/)
    {
        success = true;
        /*updateSolution(puzzle, sol);*/
    }
    else 
    {
        success = false;
    }

    /* GRBfreemodel(model);
    GRBfreeenv(env); */
    /*free(sol);*/
    /*freeVariables(blockNumOfCells);*/
    return success;
}

void fillIntSolution(Puzzle *puzzle, int *sol, Mode mode)
{
    int i, j, k, index, cellSol;
    int blockNumOfCells = puzzle->blockNumOfCells;
    Cell *cell;
    for (i=1; i<blockNumOfCells+1; i++)
    {
        for (j=1; j<blockNumOfCells+1; j++)
        {
            for (k=1; k<blockNumOfCells+1; k++)
            {
                index = variables[i-1][j-1][k-1];
                if (index)
                {
                    cellSol = sol[index-1];
                    if (cellSol)
                    {
                        setCell(puzzle, i, j, k, mode);
                    }
                }
            }
        }
    }
}

/* fiil cell <x,y> by randomly choosing a value according to 
 * values[] which is the probability
 * if none of the options is ligal don't fill it */
Move* fillCellAccordingToProb(Puzzle *puzzle, int x, int y, float *values)
{
    Move *head = NULL;
    return head;
}


Move* fillThresholdSolution(Puzzle *puzzle, int *sol, float threshold, Mode mode)
{
    Move *head = NULL;
    Move *m;
    int i, j, k, index;
    float cellSol;
    int blockNumOfCells = puzzle->blockNumOfCells;
    float *scores = (float*)calloc(blockNumOfCells, sizeof(float));
    Cell *cell;
    for (i=1; i<blockNumOfCells+1; i++)
    {
        for (j=1; j<blockNumOfCells+1; j++)
        {
            cell = getCell(puzzle, i, j);
            if (!(cell->value))
            {
                for (k=1; k<blockNumOfCells+1; k++)
                {
                    index = variables[i-1][j-1][k-1];
                    if (index)
                    {
                        cellSol = sol[index-1];
                        if (cellSol>=threshold)
                        {
                            scores[k-1] = cellSol;
                        }
                        else
                        {
                            scores[k-1] = 0;
                        }
                    }
                    else
                    {
                        scores[k-1] = 0;
                    }
                }
                m = fillCellAccordingToProb(puzzle, i, j, scores);
                concat(&head, &m);
            }
        }
    }
    free(scores);
    return head;
}

