#include "ILP.h"
#include "gurobi_c.h"

/*
 * 3d array (col*row*value)
 * each cell of possible value has index that we use later to determine its position
 * in the array that we send to gurobi. this way reduce the number of variables we send to gurobi
 */
int ***variables=NULL;

/* fiil cell <x,y> by randomly choosing a value according to
 * values[] which is the probability
 * if none of the options is ligal don't fill it
 * return the cell that changed
 */
Move *fillCellAccordingToProb(Puzzle *puzzle, int col, int row, double *valueVsScore)
{
	double sum=0, randVal=((rand()%10000)/10000.0);
	int i=0, N=puzzle->blockNumOfCells;
	Move *head=NULL;
	bool isBoardErroneous=false;

	do
	{
		isBoardErroneous=false;
		/* sum the scores */
		for (i=0;i<N;i++)
		{

			sum+=valueVsScore[i];
		}

		if (sum==0)
			return NULL;

		/* change the score of each cell to its relative probability */
		for (i=0;i<N;i++)
		{
			valueVsScore[i]=(valueVsScore[i]/sum);

		}

		/* choose value randomly */
		i=0;
		sum=0;
		while (i<N && sum<=randVal)
		{
			sum+=valueVsScore[i];
			i++; /* when exit the loop, i is the random value */
		}

		/* if the chosen value turns the board to erroneous choose another value */
		head = setCell(puzzle, col, row, i, Solve);

		if (isBoardErr(puzzle))
		{
			isBoardErroneous = true;
			deleteList(head);
			head=NULL;
			valueVsScore[i-1]=0;
		}
	}
	while (isBoardErroneous);

	if (head==NULL)
    {
        addToList(&head, col, row, 0, 0);
    }
    return head;
}

/* add variables to model */
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

/*
 * init variables array
 */
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

/* Change objective sense to maximization */
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

/*
 * free variables array
 */
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

/*
 * check if array of size N contains val
 * return true if val found, otherwise false
 */
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

/*
 * set random array values between 1-N (without repeatations)
 * N - size of array
 */
void randomCoefficients(double *array, int N)
{
	int i=0, r=0;

	do
	{
        r=(rand()%(N))+1;
		if (!inArray(r,array,N))
		{
			array[i]=r;
			i++;
		}
	}
	while(i<N);
}

/*
 * update variables array
 */
int updateVariables(Puzzle *puzzle, bool isILP, GRBmodel **model, GRBenv **env)
{
    int i=0, j=0, k=0, cnt = 1, error = 0, row=0, col=0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    double *obj=NULL; /* obj is coefficients array */
    double *upperBounds = NULL;
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


    values = (int *)calloc(blockNumOfCells + 1, sizeof(int));

    if (values == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }

    obj = (double *)calloc(maxNumOfVars, sizeof(double));

    if (obj == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(values);

        exit(0);
    }

    vtype = (char *)calloc(maxNumOfVars, sizeof(char));

    if (vtype == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(values);

        free(obj);

        exit(0);
    }

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

        upperBounds = (double*)calloc(maxNumOfVars, sizeof(double));

        if (upperBounds==NULL) /* calloc failed */
        {
            printError(MemoryAllocFailed, NULL, 0, 0);
            exit(0);
        }

        for(i=0; i<cnt; i++)
    	{

            vtype[i] = GRB_CONTINUOUS;
            upperBounds[i] = 1.0;
    	}

        randomCoefficients(obj, cnt);

    }
    


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

    if (setIntAttr(model, env) == -1)
    {
        free(values);

        free(obj);

        free(vtype);

        return -1;
    }

    free(values);

    free(obj);

    free(vtype);

    return cnt;
}

/* Add constraint to model */
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

/*
 * add constraint that each cell has only one value
 */
int addCellsConstraints(Puzzle *puzzle, GRBmodel *model, GRBenv *env)
{
    int col, row, k, cnt = 0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    char temp[13] = {0};
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

    free(ind);

    free(val);

    return 0;
}

/*
 * add constraint that each column has all possible values 1<=m*n
 */
int addColsConstraints(Puzzle *puzzle, GRBmodel *model, GRBenv *env)
{
    int row, col, k, cnt = 0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    char temp[16] = {0};
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

/*
 * add constraint that each row has all possible values 1<=m*n
 */
int addRowsConstraints(Puzzle *puzzle, GRBmodel *model, GRBenv *env)
{
    int row, col, k, cnt = 0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    char temp[16] = {0};
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

/*
 * add constraint that each block has all possible values 1<=m*n
 */
int addBlocksConstraints(Puzzle *puzzle, GRBmodel *model, GRBenv *env)
{
    int row, col, k, n, m, index=0, cnt = 0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    int blockNumOfCols = puzzle->blockNumCol;
    int blockNumOfRows = puzzle->blockNumRow;
    char temp[23] = {0};
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

/*
 * create objective function with appropriate subjects to the desired solution
 * return 1 if solution has found, 0 if not found, -1 if gurobi failed
 */
int findSolution(Puzzle *puzzle, bool isILP, int *numOfVariables, double **solPtr)
{
    GRBenv *env = NULL;
    GRBenv **envPtr=&env;
    GRBmodel *model = NULL;
    GRBmodel **modelPtr = &model;
    int error=0;
    int optimstatus=0;
    int success=0;


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

	*numOfVariables = updateVariables(puzzle, isILP, modelPtr, envPtr);


	*solPtr = (double *)calloc(*numOfVariables, sizeof(double));

	if (*solPtr == NULL) /* calloc failed */
	{
		printError(MemoryAllocFailed, NULL, 0, 0);
		exit(0);
	}

	/* update the model - to integrate new variables */
	error = GRBupdatemodel(model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}

	/* add constraints to model */
	if (addCellsConstraints(puzzle, model, env) == -1)
	{
		success = -1;
		goto END;
	}

	if (addRowsConstraints(puzzle, model, env) == -1)
	{
		success = -1;
		goto END;
	}

	if (addColsConstraints(puzzle, model, env) == -1)
	{
		success = -1;
		goto END;
	}

	if (addBlocksConstraints(puzzle, model, env) == -1)
	{
		success = -1;
		goto END;
	}

	/* Optimize model - need to call this before calculation */
	error = GRBoptimize(model);
	if (error) {
		printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}

	/* Write model to 'mip1.lp' - this is not necessary but very helpful */
	error = GRBwrite(model, "mip1.lp");
	if (error) {
		printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}

	/* Get solution information */
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}

	/* solution found */
	if (optimstatus == GRB_OPTIMAL) {
		success = 1;

	}
	/* no solution found */
	else if (optimstatus == GRB_INF_OR_UNBD) {
		success = 0;

		goto END;
	}
	/* error or calculation stopped */
	else {

		success = -1;
		goto END;
	}


	/* get the solution - the assignment to each variable */
	/* 4 - number of variables, the size of "sol" should match */
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, *numOfVariables, *solPtr);
	if (error) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}

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

	success = findSolution(puzzle, true, &numOfVariables, &sol);

	if (sol!=NULL)
    {
        free(sol);

    }
		
	freeVariables(puzzle->blockNumOfCells);

	return success;
}

/*
 * fill board with solution
 */
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
	int numOfVariables;
	double *sol = NULL;
	int success;
	success = findSolution(puzzle, true, &numOfVariables, &sol);
	if (success == 1)
	{
		fillIntSolution(puzzle, sol, Edit);
	}
	if(sol!=NULL)
    {
        free(sol);

    }
	freeVariables(puzzle->blockNumOfCells);
	return puzzle;
}

/*
 * fill board with probabilistic solution
 */
Move* fillDblSolution(Puzzle *puzzle, double threshold, double *sol)
{
    Move *head = NULL;
    Move *m = NULL;
    int row, col, k, index;
    double cellSol;
    int blockNumOfCells = puzzle->blockNumOfCells;
    double *scores = (double *)calloc(blockNumOfCells, sizeof(double));

    if (scores==NULL)/* calloc failed */
    {
    	printError(MemoryAllocFailed, NULL, 0, 0);
    	exit(0);
    }

    /* if statement added to avoid unused error (pedantic-error) */
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
                    	scores[k-1] = cellSol;
                    }
                    else
                    {
                    	scores[k-1] = 0.0;
                    }
                }
                else
                {
                	scores[k-1] = 0.0;
                }
            }

        	m = fillCellAccordingToProb(puzzle, col, row, scores);

        	concat(&head, &m);

        }
    }

    free(scores);

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

		head = fillDblSolution(puzzle, threshold, sol);
        if(head==NULL)
	    {
		    addToList(&head, 0, 0, 0, 0);
	    }

	}

	if(sol!=NULL)
    {
        free(sol);

    }

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

    success = findSolution(puzzle, false, &numOfVariables, &sol);

	if (success == 1)
	{
		for (k=1; k<puzzle->blockNumOfCells+1; k++)
        {

            index = variables[row-1][col-1][k-1];

            if (index)
            {

                values[k-1] = sol[index-1];

            }
        }
	}
	if(sol!=NULL)
    {
        free(sol);

    }
	freeVariables(puzzle->blockNumOfCells);
	return success;
}

/*
 * return value for cell <col,row> that was matched to a solution to current puzzle
 */
int ILPCellSolver(Puzzle *puzzle, int col, int row)
{
	int numOfVariables = 0;
	double *sol = NULL, isSol=0.0;
	int success=0, k=0, index=0, cellSol=0;
	success = findSolution(puzzle, true, &numOfVariables, &sol);

	if (success == 1)
	{
		for (k=1; k<puzzle->blockNumOfCells+1; k++)
        {

            if (variables[row-1][col-1][k-1] != 0)
            {
                index = variables[row-1][col-1][k-1] - 1;

                isSol = sol[index];

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
    {
        free(sol);

    }
	freeVariables(puzzle->blockNumOfCells);
	return cellSol;
}
