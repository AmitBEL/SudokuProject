#include "ILP.h"
#include "gurobi_c.h"

int ***variables=NULL;

int addVars(GRBmodel **model, GRBenv **env, int numOfVarsToAdd, double *obj, char *vtype)
{
    int error = 0;
    double upperBound=1.0;
    error = GRBaddvars(*model, numOfVarsToAdd, 0, NULL, NULL, NULL, obj, NULL, &upperBound, vtype, NULL);
    if (error)
    {
        printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(*env));
        return -1;
    }
    return 0;
}

void initVariables(int blockNumOfCells)
{
    int i, j, k, l;

    variables = (int ***)calloc(blockNumOfCells, sizeof(int **));
    if (variables == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }

    for (i = 0; i < blockNumOfCells; i++)
    {
        variables[i] = (int **)calloc(blockNumOfCells, sizeof(int *));
        if (variables[i] == NULL) /* calloc failed */
        {
            for (j = 0; j < i; j++)
            {
                free(variables[j]);
            }
            free(variables);
            printError(MemoryAllocFailed, NULL, 0, 0);
            exit(0);
        }
    }

    for (i = 0; i < blockNumOfCells; i++)
    {
        variables[i] = (int **)calloc(blockNumOfCells, sizeof(int *));
        if (variables[i] == NULL) /* calloc failed */
        {
            for (k = 0; k < i; k++)
            {
                for (l = 0; l < blockNumOfCells; l++)
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
            for (j = 0; j < blockNumOfCells; j++)
            {
                variables[i][j] = (int *)calloc(blockNumOfCells, sizeof(int));
                if (variables[i][j] == NULL) /* calloc failed */
                {
                    for (l = 0; l < j; l++)
                    {
                        free(variables[i][l]);
                    }
                    free(variables[i]);
                    for (k = 0; k < i; k++)
                    {
                        for (l = 0; l < blockNumOfCells; l++)
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

int setIntAttr(GRBmodel **model, GRBenv **env)
{
    int error = 0;
    error = GRBsetintattr(*model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
    if (error)
    {
        printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(*env));
        return -1;
    }
    return 0;
}

void freeVariables(int blockNumOfCells)
{
    int i, j /*, k*/;
    for (i = 0; i < blockNumOfCells; i++)
    {
        for (j = 0; j < blockNumOfCells; j++)
        {
            free(variables[i][j]);
        }
        free(variables[i]);
    }
    free(variables);
}

bool inArray(int val, double *array, int N)
{
	int i=0;

	for (i=0;i<N;i++)
	{
		if (array[i]==val)
			return true;
	}

	return false;
}

void randomCoefficients(double *array, int N)
{
	int i=0, r=0;

	do
	{
		r=(rand()%(N-i))+1;
		if (!inArray(r,array,N))
		{
			array[i]=r;
			i++;
		}
	}
	while(i<N);
}

int updateVariables(Puzzle *puzzle, bool isILP, GRBmodel **model, GRBenv **env)
{
    int i=0, j=0, k=0, cnt = 1, error = 0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    double *obj=NULL, upperBound=1.0; /* obj is coefficients array */
    double *upperBoundPtr=&upperBound;
    int *values=NULL; /* array of legal values of cell */
    char *vtype=NULL; /* binary/continuous */
    Cell *cell=NULL;
    int maxNumOfVars = blockNumOfCells*blockNumOfCells*blockNumOfCells;

    print("A");
    printf("blockNumOfCells=%d\n", blockNumOfCells);
    values = (int *)calloc(blockNumOfCells + 1, sizeof(int));
    if (values == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    print("B");
    obj = (double *)calloc(maxNumOfVars, sizeof(double));
    if (obj == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(values);
        exit(0);
    }
    print("C");
    vtype = (char *)calloc(maxNumOfVars, sizeof(char));
    if (vtype == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(values);
        free(obj);
        exit(0);
    }
    print("D");
    for (i = 1; i < blockNumOfCells + 1; i++)
    {
        for (j = 1; j < blockNumOfCells + 1; j++)
        {
            cell = getCell(puzzle, i, j);
            if (!(cell->value))
            {
                values = numOfCellSol(puzzle, i, j, values);
                for (k = 1; k < blockNumOfCells + 1; k++)
                {
                    if (values[k])
                    {
                        variables[i - 1][j - 1][k - 1] = cnt;

                        /*if (isILP)
                        {
                            obj[cnt - 1] = 1;
                            vtype[cnt - 1] = GRB_BINARY;
                        }
                        else
                        {
                            obj[cnt - 1] = randomCoefficient(blockNumOfCells);
                            vtype[cnt - 1] = GRB_CONTINUOUS;
                        }*/
                        cnt++;
                    }
                }
            }
        }
    }
    cnt--;
    if (isILP)
    {
    	for(i=0; i<cnt; i++)
    	{
    		obj[i] = 1.0;
    		vtype[i] = GRB_BINARY;
    	}
    }
    else
    {
    	for(i=0; i<cnt; i++)
    	{
    		vtype[i] = GRB_CONTINUOUS;
    	}
    	randomCoefficients(obj, cnt);
    	for(i=0; i<cnt; i++)
		{
			printf("%f, ", obj[i]);
		}
    	printf("\n");
    }
    print("E");
    /*printf("*model=%d, cnt=%d, 3rd=%d, 4th=NULL, 5th=NULL 6th=NULL, obj=%f, 8th=NULL, upperBound=%f, vtype=%c, 11th=NULL\n", (*model==NULL?0:1), cnt, 0, obj[cnt-1], *upperBoundPtr, vtype[cnt-1]);*/
    error = GRBaddvars(*model, cnt, 0, NULL, NULL, NULL, obj, NULL, upperBoundPtr, vtype, NULL);
	if (error)
	{
		printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(*env));
		free(values);
		free(obj);
		free(vtype);
		return -1;
	}
    print("F");
    if (setIntAttr(model, env) == -1)
    {
        free(values);
        free(obj);
        free(vtype);
        return -1;
    }
    print("G");
    free(values);
    free(obj);
    free(vtype);
    return cnt;
}

bool findSolution(Puzzle *puzzle, bool isILP, int *numOfVariables, double *sol)
{
    GRBenv *env = NULL;
    GRBenv **envPtr=&env;
    GRBmodel *model = NULL;
    GRBmodel **modelPtr = &model;
    int error=0;
    if (puzzle==NULL && isILP==true && numOfVariables==NULL && sol==NULL)
    	return false;
    /*int optimstatus;
    bool success;
    double objval;*/

    /* Create environment - log file is mip1.log */
	error = GRBloadenv(&env, "mip1.log");
	if (error) {
		printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
		return false;
	}

	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
		printf("ERROR %d GRBsetintparam(): %s\n", error, GRBgeterrormsg(env));
		return false;
	}

	/* Create an empty model named "mip1" */
	error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
	if (error) {
		printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
		return false;
	}

	/* Add variables */
	initVariables(puzzle->blockNumOfCells);

	/* Change objective sense to maximization */
	printf("dvir\n");
	*numOfVariables = updateVariables(puzzle, isILP, modelPtr, envPtr);
	printf("dvir\n");

	return true;
}

/* run ILP and return if there is a solution or not */
bool ILPSolvable(Puzzle *puzzle)
{
	int numOfVariables;
	double *sol = NULL;
	bool success;
	print("in ILPSolvable");
	success = findSolution(puzzle, true, &numOfVariables, sol);
	print("after findSolution");
	if (sol!=NULL)
		free(sol);
	/*freeVariables(puzzle->blockNumOfCells);*/
	print("after freeVariables");
	return success;
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
Move* LPSolver(Puzzle *puzzle, double threshold)
{
    if (puzzle==NULL && threshold != 0)
    {
        return NULL;
    }
    return NULL;
}

/* 
 * return a list such that in index i there is the
 * probability that the value of cell is i+1 
 */
double* LPCellValues(Puzzle *puzzle, double threshold, int x, int y, double *values)
{
    if(puzzle==NULL && threshold==0.0 && x==0 && y==0 && values==NULL)
    	return values;
    return NULL;
}

int ILPCellSolver(Puzzle *puzzle, int x, int y)
{
	if (puzzle==NULL && x==0 && y==0)
		return 0;
	return 0;
}
