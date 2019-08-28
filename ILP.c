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
    int row, col, k, l;

    variables = (int ***)calloc(blockNumOfCells, sizeof(int **));
    if (variables == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }

    for (row = 0; row < blockNumOfCells; row++)
    {
        variables[row] = (int **)calloc(blockNumOfCells, sizeof(int *));
        if (variables[row] == NULL) /* calloc failed */
        {
            for (col = 0; col < row; col++)
            {
                free(variables[col]);
            }
            free(variables);
            printError(MemoryAllocFailed, NULL, 0, 0);
            exit(0);
        }
    }

    for (row = 0; row < blockNumOfCells; row++)
    {
        variables[row] = (int **)calloc(blockNumOfCells, sizeof(int *));
        if (variables[row] == NULL) /* calloc failed */
        {
            for (k = 0; k < row; k++)
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
            for (col = 0; col < blockNumOfCells; col++)
            {
                variables[row][col] = (int *)calloc(blockNumOfCells, sizeof(int));
                if (variables[row][col] == NULL) /* calloc failed */
                {
                    for (l = 0; l < col; l++)
                    {
                        free(variables[row][l]);
                    }
                    free(variables[row]);
                    for (k = 0; k < row; k++)
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
    int row, col;
    if (variables==NULL)
    	return;
    for (row = 0; row < blockNumOfCells; row++)
    {
        for (col = 0; col < blockNumOfCells; col++)
        {
            free(variables[row][col]);
        }
        free(variables[row]);
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
		printf("i=%d\n", i);
        r=(rand()%(N))+1;
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
    int i=0, j=0, k=0, cnt = 1, error = 0, row=0, col=0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    double *obj=NULL; /* obj is coefficients array */
    double *upperBounds = NULL /*= (isILP ? NULL : &upperBound)*/;
    int *values=NULL; /* array of legal values of cell */
    char *vtype=NULL; /* binary/continuous */
    Cell *cell=NULL;
    int maxNumOfVars = blockNumOfCells*blockNumOfCells*blockNumOfCells;
    char **varsNames = (char**)calloc(maxNumOfVars, sizeof(char*));

    if (varsNames == NULL)
    {
        free(varsNames);
        printError(MemoryAllocFailed, NULL, 0 ,0);
        exit(0);
    }

    for (i=0; i<maxNumOfVars; i++)
    {
        varsNames[i] = (char*)calloc(10, sizeof(char));
        if (varsNames[i] == NULL) /* calloc failed */
        {
            for (j=0; j<i; j++)
            {
                free(varsNames[j]);
            }
            free(varsNames);
            printError(MemoryAllocFailed, NULL, 0 ,0);
            exit(0);
        }
    }

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
    for (row = 1; row < blockNumOfCells + 1; row++)
    {
        for (col = 1; col < blockNumOfCells + 1; col++)
        {
            cell = getCell(puzzle, col, row);
            if (!(cell->value))
            {
                values = numOfCellSol(puzzle, col, row, values);
                for (k = 1; k < blockNumOfCells + 1; k++)
                {
                    if (values[k])
                    {
                        variables[row - 1][col - 1][k - 1] = cnt;
                        sprintf(varsNames[cnt - 1], "x%d%d%d", col, row, k);
                        cnt++;
                    }
                }
            }
        }
    }
    cnt--;
    printf("E");
    if (isILP)
    {
    	printf("F");
        for(i=0; i<cnt; i++)
    	{
    		obj[i] = 1.0;
    		vtype[i] = GRB_BINARY;
    	}
    }
    else
    {
    	printf("G");
        upperBounds = (double*)calloc(maxNumOfVars, sizeof(double));
        if (upperBounds==NULL) /* calloc failed */
        {
            printError(MemoryAllocFailed, NULL, 0, 0);
            exit(0);
        }

        for(i=0; i<cnt; i++)
    	{
    		/* obj[i] = 1.0;   temp coef - suppose to be random */
            vtype[i] = GRB_CONTINUOUS;
            upperBounds[i] = 1.0;
    	}
    	printf("before randomCoefficients\n");
        randomCoefficients(obj, cnt);
        printf("after randomCoefficients\n");
    	/*for(i=0; i<cnt; i++)
		{
			printf("%f, ", obj[i]);
		}
    	printf("\n");*/
    }
    print("H");
    
    /*printf("*model=%d, cnt=%d, 3rd=%d, 4th=NULL, 5th=NULL 6th=NULL, obj=%f, 8th=NULL, upperBound=%f, vtype=%c, 11th=NULL\n", (*model==NULL?0:1), cnt, 0, obj[cnt-1], (upperBoundPtr==NULL?0:*upperBoundPtr), vtype[cnt-1]);*/  
    /*printf("upper=%f, vtype[cnt-1]=%c\n", *upperBoundPtr, vtype[cnt-1]);*/
    error = GRBaddvars(*model, cnt, 0, NULL, NULL, NULL, obj, NULL, upperBounds, vtype, varsNames);

    for (i=0; i<maxNumOfVars; i++)
    {
        free(varsNames[i]);
    }
    free(varsNames);

    if (upperBounds!=NULL)
    {
        free(upperBounds);
    }

	if (error)
	{
		printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(*env));
		free(values);
		free(obj);
		free(vtype);
		return -1;
	}
    print("I");
    if (setIntAttr(model, env) == -1)
    {
        free(values);
        free(obj);
        free(vtype);
        return -1;
    }
    print("J");
    free(values);
    free(obj);
    free(vtype);
    return cnt;
}

int addConstraint(GRBmodel *model, GRBenv *env, int numOfVars, int *ind, double *val, char *consName)
{
	int error = 0;
	if (numOfVars == 0)
		return 0;
    error = GRBaddconstr(model, numOfVars, ind, val, GRB_EQUAL, 1.0, consName);
    if (error)
    {
        printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }
    return 0;
}

int addCellsConstraints(Puzzle *puzzle, GRBmodel *model, GRBenv *env)
{
    int col, row, k, cnt = 0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    char temp[20] = {0};
    int *ind;
    double *val;
    print("a");
    ind = (int *)calloc(blockNumOfCells, sizeof(int));
    if (ind == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    print("b");
    val = (double *)calloc(blockNumOfCells, sizeof(double));
    if (val == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(ind);
        exit(0);
    }
    print("c");
    for (row = 1; row < blockNumOfCells + 1; row++)
    {
        for (col = 1; col < blockNumOfCells + 1; col++)
        {
            for (k = 1; k < blockNumOfCells + 1; k++)
            {
                if (variables[row - 1][col - 1][k - 1])
                {
                    ind[cnt] = variables[row - 1][col - 1][k - 1] - 1;
                    val[cnt] = 1;
                    cnt++;
                }
            }
            sprintf(temp, "cell <%d,%d>", col, row);
            if (addConstraint(model, env, cnt, ind, val, temp) == -1)
            {
                free(ind);
				free(val);
				return -1;
            }
            cnt = 0;
        }
    }
    print("d");
    free(ind);
    free(val);
    return 0;
}

int addColsConstraints(Puzzle *puzzle, GRBmodel *model, GRBenv *env)
{
    int row, col, k, cnt = 0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    char temp[20] = {0};
    int *ind;
    double *val;
    ind = (int *)calloc(blockNumOfCells, sizeof(int));
    if (ind == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    val = (double *)calloc(blockNumOfCells, sizeof(double));
    if (val == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(ind);
        exit(0);
    }
    for (col = 1; col < blockNumOfCells + 1; col++)
    {
        for (k = 1; k < blockNumOfCells + 1; k++)
        {
            for (row = 1; row < blockNumOfCells + 1; row++)
            {
                if (variables[row - 1][col - 1][k - 1])
                {
                    ind[cnt] = variables[row - 1][col - 1][k - 1] - 1;
                    val[cnt] = 1.0;
                    cnt++;
                }
            }
            sprintf(temp, "col:%d value:%d", col, k);
            if (addConstraint(model, env, cnt, ind, val, temp) == -1)
            {
                free(ind);
				free(val);
				return -1;
            }
            cnt = 0;
        }
    }
    free(ind);
    free(val);
    return 0;
}

int addRowsConstraints(Puzzle *puzzle, GRBmodel *model, GRBenv *env)
{
    int row, col, k, cnt = 0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    char temp[20] = {0};
    int *ind;
    double *val;
    ind = (int *)calloc(blockNumOfCells, sizeof(int));
    if (ind == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    val = (double *)calloc(blockNumOfCells, sizeof(double));
    if (val == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(ind);
        exit(0);
    }
    for (row = 1; row < blockNumOfCells + 1; row++)
    {
        for (k = 1; k < blockNumOfCells + 1; k++)
        {
            for (col = 1; col < blockNumOfCells + 1; col++)
            {
                if (variables[row - 1][col - 1][k - 1])
                {
                    ind[cnt] = variables[row - 1][col - 1][k - 1] - 1;
                    val[cnt] = 1;
                    cnt++;
                }
            }
            sprintf(temp, "row:%d value:%d", row, k);
            if (addConstraint(model, env, cnt, ind, val, temp) == -1)
            {
                free(ind);
				free(val);
				return -1;
            }
            cnt = 0;
        }
    }
    free(ind);
    free(val);
    return 0;
}

int addBlocksConstraints(Puzzle *puzzle, GRBmodel *model, GRBenv *env)
{
    int row, col, k, n, m, index=0, cnt = 0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    int blockNumOfCols = puzzle->blockNumCol;
    int blockNumOfRows = puzzle->blockNumRow;
    char temp[20] = {0};
    int *ind;
    double *val;
    ind = (int *)calloc(blockNumOfCells, sizeof(int));
    if (ind == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    val = (double *)calloc(blockNumOfCells, sizeof(double));
    if (val == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(ind);
        exit(0);
    }
    for (n = 0; n < blockNumOfRows; n++)
    {
        for (m = 0; m < blockNumOfCols; m++)
        {
            for (k = 1; k < blockNumOfCells + 1; k++)
            {
                for (col = 1; col < blockNumOfCols + 1; col++)
                {
                    for (row = 1; row < blockNumOfRows + 1; row++)
                    {
                        /*printf("[(%d*3)+(%d-1)][(%d*2)+(%d-1)][%d-1] = [%d][%d][%d]\n",m,row,n,col,k, (m*blockNumOfRows)+(row-1), (n*blockNumOfCols)+(col-1),k-1);*/
                        index = variables[(m*blockNumOfRows)+(row-1)][(n*blockNumOfCols)+(col-1)][k-1];
                        if (index)
                        {
                            ind[cnt] = index - 1;
                            val[cnt] = 1;
                            cnt++;
                        }
                    }
                }
                sprintf(temp, "block:<%d,%d> value:%d", n+1, m+1, k);
                if (addConstraint(model, env, cnt, ind, val, temp) == -1)
                {
                    free(ind);
                    free(val);
                    return -1;
                }
                cnt = 0;
            }
        }
    }
    free(ind);
    free(val);
    return 0;
}

int findSolution(Puzzle *puzzle, bool isILP, int *numOfVariables, double **solPtr)
{
    GRBenv *env = NULL;
    GRBenv **envPtr=&env;
    GRBmodel *model = NULL;
    GRBmodel **modelPtr = &model;
    int error=0;
    int optimstatus=0;
    int success=0;
    double objval=0;

    /* if statement added to avoid unused error (pedantic-error) */
	if (puzzle==NULL && isILP==true && numOfVariables==NULL && *solPtr==NULL)
		return 0;

    /* Create environment - log file is mip1.log */
	error = GRBloadenv(&env, "mip1.log");
	if (error) {
		printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}

	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
		printf("ERROR %d GRBsetintparam(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}

	/* Create an empty model named "mip1" */
	error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
	if (error) {
		printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}

	/* Add variables */
	initVariables(puzzle->blockNumOfCells);

	/* Change objective sense to maximization */
	print("1");
	*numOfVariables = updateVariables(puzzle, isILP, modelPtr, envPtr);
	print("2");

	*solPtr = (double *)calloc(*numOfVariables, sizeof(double));
	if (*solPtr == NULL) /* calloc failed */
	{
		printError(MemoryAllocFailed, NULL, 0, 0);
		exit(0);
	}
	print("3");
	/* update the model - to integrate new variables */
	error = GRBupdatemodel(model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}
	print("4");
	/* add constraints to model */
	if (addCellsConstraints(puzzle, model, env) == -1)
	{
		success = -1;
		goto END;
	}
	print("5");
	if (addRowsConstraints(puzzle, model, env) == -1)
	{
		success = -1;
		goto END;
	}
	print("6");
	if (addColsConstraints(puzzle, model, env) == -1)
	{
		success = -1;
		goto END;
	}
	print("7");
	if (addBlocksConstraints(puzzle, model, env) == -1)
	{
		success = -1;
		goto END;
	}
	print("8");
	/* Optimize model - need to call this before calculation */
	error = GRBoptimize(model);
	if (error) {
		printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}
	print("9");
	/* Write model to 'mip1.lp' - this is not necessary but very helpful */
	error = GRBwrite(model, "mip1.lp");
	if (error) {
		printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}
	print("10");
	/* Get solution information */
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}
	print("11");
	/* get the objective -- the optimal result of the function */
	/* not necessary but may be harmful to gurobi using */
	/*error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
	if (error) {
		printf("ERROR %d GRBgetdblattr(): %s\n", error, GRBgeterrormsg(env));
		return false;
	}*/

	/* print results */
	printf("\nOptimization complete\n");

	/* solution found */
	if (optimstatus == GRB_OPTIMAL) {
		success = 1;
		printf("Optimal objective: %.4e\n", objval);
	}
	/* no solution found */
	else if (optimstatus == GRB_INF_OR_UNBD) {
		success = 0;
		printf("Model is infeasible or unbounded\n");
		goto END;
	}
	/* error or calculation stopped */
	else {
		printf("Optimization was stopped early\n");
		success = -1;
		goto END;
	}

	print("beginning of if (success)");
	/* get the solution - the assignment to each variable */
	/* 3-- number of variables, the size of "sol" should match */
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, *numOfVariables, *solPtr);
	if (error) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}
    printf("sol=%d\n", *solPtr==NULL?0:1);
    printf("sol[0]=%f\n", *solPtr[0]);
	print("end of if (success)");

	print("12");

	/* IMPORTANT !!! - Free model and environment */

	END:
	if (model!=NULL)
		GRBfreemodel(model);
	if (env!=NULL)
		GRBfreeenv(env);

	return success;
}

/* run ILP and return if there is a solution or not */
int ILPSolvable(Puzzle *puzzle)
{
	int numOfVariables;
	double *sol = NULL;
	int success = 0;
	print("in ILPSolvable");
	success = findSolution(puzzle, true, &numOfVariables, &sol);
	print("after findSolution");
	if (sol!=NULL)
		free(sol);
	freeVariables(puzzle->blockNumOfCells);
	print("after freeVariables");
	return success;
}

void fillIntSolution(Puzzle *puzzle, double *sol, Mode mode)
{
	Move *dummyMove=NULL;
    int row, col, k, index, cellSol;
    int blockNumOfCells = puzzle->blockNumOfCells;
    for (row = 1; row < blockNumOfCells + 1; row++)
    {
        for (col = 1; col < blockNumOfCells + 1; col++)
        {
            for (k = 1; k < blockNumOfCells + 1; k++)
            {
                index = variables[row - 1][col - 1][k - 1];
                if (index)
                {
                    cellSol = sol[index - 1];
                    if (cellSol)
                    {
                    	dummyMove = setCell(puzzle, col, row, k, mode);
                    	deleteList(dummyMove);
                    }
                }
            }
        }
    }
}

/* run ILP and return a solution */
Puzzle* ILPSolver(Puzzle *puzzle)
{
	int numOfVariables /*, value=0, index, k*/;
	double *sol = NULL;
	int success;
	success = findSolution(puzzle, true, &numOfVariables, &sol);
	if (success == 1)
	{
		fillIntSolution(puzzle, sol, Edit);
	}
	if(sol!=NULL)
		free(sol);
	freeVariables(puzzle->blockNumOfCells);
	return puzzle;
}

Move* fillDblSolution(Puzzle *puzzle, double threshold, double *sol, Mode mode)
{
    Move *head = NULL;
    Move *m = NULL;
    int row, col, k, index;
    double cellSol;
    int blockNumOfCells = puzzle->blockNumOfCells;

    if(head==NULL)
    {
        head = NULL;
    }
    for (row = 1; row < blockNumOfCells + 1; row++)
    {
        for (col = 1; col < blockNumOfCells + 1; col++)
        {
            for (k = 1; k < blockNumOfCells + 1; k++)
            {
                index = variables[row - 1][col - 1][k - 1];
                if (index)
                {
                    cellSol = sol[index - 1];
                    if (cellSol>=threshold)
                    {
                    	printf("cell <%d,%d> cellSol is %f\n", col, row, cellSol);
                        m = setCell(puzzle, col, row, k, mode);
                    	concat(&head, &m);
                    }
                }
            }
        }
    }
    return head;
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
    int numOfVariables;
	double *sol = NULL;
	int success;
    Move* head = NULL;
	success = findSolution(puzzle, false, &numOfVariables, &sol);
	if (success == 1)
	{
		head = fillDblSolution(puzzle, threshold, sol, Solve);
	}
	if(sol!=NULL)
		free(sol);
	freeVariables(puzzle->blockNumOfCells);
	return head;
}
/* 
 * return a list such that in index i there is the
 * probability that the value of cell is i+1 
 */
int LPCellValues(Puzzle *puzzle, int col, int row, double *values)
{
    int numOfVariables = 0;
	double *sol = NULL;
	int success=0, k=0, index=0;
	printf("in LPCellValues\n");
    success = findSolution(puzzle, false, &numOfVariables, &sol);
    printf("success=%d\n", success);
    printf("numOfVariables=%d\n", numOfVariables);
    printf("sol=%d\n", sol==NULL?0:1);
	if (success == 1)
	{
		for (k=1; k<puzzle->blockNumOfCells+1; k++)
        {
            printf("k=%d\n", k);
            index = variables[row-1][col-1][k-1];
            printf("variables[%d][%d][%d]=%d\n", row-1, col-1, k-1, index);
            if (index)
            {
                printf("index=%d\n", index);
                values[k-1] = sol[index-1];
                printf("values[%d]=%f\n", k-1, values[k-1]);
            }
        }
	}
	if(sol!=NULL)
		free(sol);
	freeVariables(puzzle->blockNumOfCells);
	return success;
}

int ILPCellSolver(Puzzle *puzzle, int col, int row)
{
	int numOfVariables = 0;
	double *sol = NULL, isSol=0.0;
	int success=0, k=0, index=0, cellSol=0;
	success = findSolution(puzzle, true, &numOfVariables, &sol);
    printf("success=%d\n", success);
    printf("numOfVariables=%d\n", numOfVariables);
    printf("sol=%d\n", sol==NULL?0:1);
	if (success == 1)
	{
		for (k=1; k<puzzle->blockNumOfCells+1; k++)
        {
            printf("k=%d\n", k);
            printf("variables[%d][%d][%d]=%d\n", row-1, col-1, k-1, variables[row-1][col-1][k-1]);
            if (variables[row-1][col-1][k-1] != 0)
            {
                index = variables[row-1][col-1][k-1] - 1;
                printf("index=%d\n", index);
                isSol = sol[index];
                printf("isSol=%f\n", isSol);
                if (isSol == 1.0)
                {
                    cellSol = k;
                    break;
                }
            }
        }
	}
    else
    {
        cellSol = success;
    }
	if(sol!=NULL)
		free(sol);
	freeVariables(puzzle->blockNumOfCells);
	return cellSol;
}
