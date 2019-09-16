#include "ILP.h"
#include "gurobi_c.h"

int ***variables=NULL;

/* fiil cell <x,y> by randomly choosing a value according to
 * values[] which is the probability
 * if none of the options is ligal don't fill it */
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
			/*printf("i=%d, score=%f\n", i, valueVsScore[i]);*/
			sum+=valueVsScore[i];
		}
		/*print("-");*/
		if (sum==0)
			return NULL;
		/*printf("sum=%f, randVal=%f\n", sum, randVal);*/
		/*print("-");*/
		/* change the score of each cell to its relative probability */
		for (i=0;i<N;i++)
		{
			valueVsScore[i]=(valueVsScore[i]/sum);
			/*printf("i=%d, score=%f\n", i, valueVsScore[i]);*/
		}

		/* choose value randomly */
		i=0;
		sum=0;
		while (i<N && sum<=randVal)
		{
			sum+=valueVsScore[i];
			i++; /* when exit the loop, i is the random value */
		}
		/*printf("sum=%f, randVal=%f\n", sum, randVal);*/
		/*printf("chosen value for cell <%d,%d> is %d\n", col, row, i);*/
		/* if the chosen value turns the board to erroneous choose another value */
		head = setCell(puzzle, col, row, i, Solve);
		/*print("after setCell");*/
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
    /*printf("15. calloc int ***variables - initVariables, ILP\n");*/
    if (variables == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }

    /*for (row = 0; row < blockNumOfCells; row++)
    {
        variables[row] = (int **)calloc(blockNumOfCells, sizeof(int *));
        printf("16. calloc int **variables[%d] - initVariables, ILP\n", row);
        if (variables[row] == NULL)  calloc failed 
        {
            for (col = 0; col < row; col++)
            {
                free(variables[col]);
            }
            free(variables);
            printError(MemoryAllocFailed, NULL, 0, 0);
            exit(0);
        }
    }*/

    for (row = 0; row < blockNumOfCells; row++)
    {
        variables[row] = (int **)calloc(blockNumOfCells, sizeof(int *));
        /*printf("16. calloc int **variables[%d] - initVariables, ILP\n", row);*/
        if (variables[row] == NULL) /* calloc failed */
        {
            for (k = 0; k < row; k++)
            {
                for (l = 0; l < blockNumOfCells; l++)
                {
                    free(variables[k][l]);
                    /*printf("17. free int *variables[%d][%d] - initVariables, ILP\n", k, l);*/
                }
                free(variables[k]);
                /*printf("16. free int **variables[%d] - initVariables, ILP\n", k);*/
            }
            free(variables);
            /*printf("15. free int ***variables - initVariables, ILP\n");*/
            printError(MemoryAllocFailed, NULL, 0, 0);
            exit(0);
        }
        else
        {
            for (col = 0; col < blockNumOfCells; col++)
            {
                variables[row][col] = (int *)calloc(blockNumOfCells, sizeof(int));
                /*printf("17. calloc int *variables[%d][%d] - initVariables, ILP\n", row, col);*/
                if (variables[row][col] == NULL) /* calloc failed */
                {
                    for (l = 0; l < col; l++)
                    {
                        free(variables[row][l]);
                        /*printf("17. free int *variables[%d][%d] - initVariables, ILP\n", row, l);*/
                    }
                    free(variables[row]);
                    /*printf("16. free int **variables[%d] - initVariables, ILP\n", row);*/
                    for (k = 0; k < row; k++)
                    {
                        for (l = 0; l < blockNumOfCells; l++)
                        {
                            free(variables[k][l]);
                            /*printf("17. free int *variables[%d][%d] - initVariables, ILP\n", k, l);*/
                        }
                        free(variables[k]);
                        /*printf("16. free int **variables[%d] - initVariables, ILP\n", k);*/
                    }
                    free(variables);
                    /*printf("15. free int ***variables - initVariables, ILP\n");*/
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
            /*printf("17. free int *variables[%d][%d] - freeVariables, ILP\n", row, col);*/
        }
        free(variables[row]);
        /*printf("16. free int **variables[%d] - freeVariables, ILP\n", row);*/
    }
    free(variables);
    /*printf("15. free int ***variables - freeVariables, ILP\n");*/
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
		/*printf("i=%d\n", i);*/
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
    /*printf("18. calloc char **varNames - updateVariables, ILP\n");*/

    if (varsNames == NULL)
    {
        free(varsNames);
        /*printf("18. free char **varsNames - updateVariables, ILP\n");*/
        printError(MemoryAllocFailed, NULL, 0 ,0);
        exit(0);
    }

    for (i=0; i<maxNumOfVars; i++)
    {
        varsNames[i] = (char*)calloc(10, sizeof(char));
        /*printf("19. calloc char *varsNames[%d] - updateVariables, ILP\n", i);*/
        if (varsNames[i] == NULL) /* calloc failed */
        {
            for (j=0; j<i; j++)
            {
                free(varsNames[j]);
                /*printf("19. free char *varsNames[%d] - updateVariables, ILP\n", j);*/
            }
            free(varsNames);
            /*printf("18. free char **varsNames - updateVariables, ILP\n");*/
            printError(MemoryAllocFailed, NULL, 0 ,0);
            exit(0);
        }
    }

    /*print("A");*/
    /*printf("blockNumOfCells=%d\n", blockNumOfCells);*/
    values = (int *)calloc(blockNumOfCells + 1, sizeof(int));
    /*printf("20. calloc int *values - updateVariables, ILP\n");*/
    if (values == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    /*print("B");*/
    obj = (double *)calloc(maxNumOfVars, sizeof(double));
    /*printf("21. calloc double *obj - updateVariables, ILP\n");*/
    if (obj == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(values);
        /*printf("20. free int *values - updateVariables, ILP\n");*/
        exit(0);
    }
    /*print("C");*/
    vtype = (char *)calloc(maxNumOfVars, sizeof(char));
    /*printf("22. calloc char *vtype - updateVariables, ILP\n");*/
    if (vtype == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(values);
        /*printf("20. free int *values - updateVariables, ILP\n");*/
        free(obj);
        /*printf("21. free double *obj - updateVariables, ILP\n");*/
        exit(0);
    }
    /*print("D");*/
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
    /*printf("E");*/
    if (isILP)
    {
    	/*printf("F");*/
        for(i=0; i<cnt; i++)
    	{
    		obj[i] = 1.0;
    		vtype[i] = GRB_BINARY;
    	}
    }
    else
    {
    	/*printf("G");*/
        upperBounds = (double*)calloc(maxNumOfVars, sizeof(double));
        /*printf("23. calloc double *upperBounds - updateVariables, ILP\n");*/
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
    	/*printf("before randomCoefficients\n");*/
        randomCoefficients(obj, cnt);
        /*printf("after randomCoefficients\n");*/
    	/*for(i=0; i<cnt; i++)
		{
			printf("%f, ", obj[i]);
		}
    	printf("\n");*/
    }
    /*print("H");*/
    
    /*printf("*model=%d, cnt=%d, 3rd=%d, 4th=NULL, 5th=NULL 6th=NULL, obj=%f, 8th=NULL, upperBound=%f, vtype=%c, 11th=NULL\n", (*model==NULL?0:1), cnt, 0, obj[cnt-1], (upperBoundPtr==NULL?0:*upperBoundPtr), vtype[cnt-1]);*/  
    /*printf("upper=%f, vtype[cnt-1]=%c\n", *upperBoundPtr, vtype[cnt-1]);*/
    error = GRBaddvars(*model, cnt, 0, NULL, NULL, NULL, obj, NULL, upperBounds, vtype, varsNames);

    for (i=0; i<maxNumOfVars; i++)
    {
        free(varsNames[i]);
        /*printf("19. free char *varsNames[%d] - updateVariables, ILP\n", i);*/
    }
    free(varsNames);
    /*printf("18. free char **varsNames - updateVariables, ILP\n");*/

    if (upperBounds!=NULL)
    {
        free(upperBounds);
        /*printf("23. free double *upperBounds - updateVariables, ILP\n");*/
    }

	if (error)
	{
		printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(*env));
		free(values);
        /*printf("20. free int *values - updateVariables, ILP\n");*/
		free(obj);
        /*printf("21. free double *obj - updateVariables, ILP\n");*/
		free(vtype);
        /*printf("22. free char *vtype - updateVariables, ILP\n");*/
		return -1;
	}
    /*print("I");*/
    if (setIntAttr(model, env) == -1)
    {
        free(values);
        /*printf("20. free int *values - updateVariables, ILP\n");*/
        free(obj);
        /*printf("21. free double *obj - updateVariables, ILP\n");*/
        free(vtype);
        /*printf("22. free char *vtype - updateVariables, ILP\n");*/
        return -1;
    }
    /*print("J");*/
    free(values);
    /*printf("20. free int *values - updateVariables, ILP\n");*/
    free(obj);
    /*printf("21. free double *obj - updateVariables, ILP\n");*/
    free(vtype);
    /*printf("22. free char *vtype - updateVariables, ILP\n");*/
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
    /*print("a");*/
    ind = (int *)calloc(blockNumOfCells, sizeof(int));
    /*printf("24. calloc int *ind - addCellsConstraints, ILP\n");*/
    if (ind == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    /*print("b");*/
    val = (double *)calloc(blockNumOfCells, sizeof(double));
    /*printf("25. calloc double *val - addCellsConstraints, ILP\n");*/
    if (val == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(ind);
        /*printf("24. free int *ind - addCellsConstraints, ILP\n");*/
        exit(0);
    }
    /*print("c");*/
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
                /*printf("24. free int *ind - addCellsConstraints, ILP\n");*/
				free(val);
                /*printf("25. free double *val - addCellsConstraints, ILP\n");*/
				return -1;
            }
            cnt = 0;
        }
    }
    /*print("d");*/
    free(ind);
    /*printf("24. free int *ind - addCellsConstraints, ILP\n");*/
    free(val);
    /*printf("25. free double *val - addCellsConstraints, ILP\n");*/
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
    /*printf("26. calloc int *ind - addColsConstraints, ILP\n");*/
    if (ind == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    val = (double *)calloc(blockNumOfCells, sizeof(double));
    /*printf("27. calloc double *val - addColsConstraints, ILP\n");*/
    if (val == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(ind);
        /*printf("26. free int *ind - addColsConstraints, ILP\n");*/
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
                /*printf("26. free int *ind - addColsConstraints, ILP\n");*/
				free(val);
                /*printf("27. free double *val - addColsConstraints, ILP\n");*/
				return -1;
            }
            cnt = 0;
        }
    }
    free(ind);
    /*printf("26. free int *ind - addColsConstraints, ILP\n");*/
    free(val);
    /*printf("27. free double *val - addColsConstraints, ILP\n");*/
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
    /*printf("28. calloc int *ind - addRowsConstraints, ILP\n");*/
    if (ind == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    val = (double *)calloc(blockNumOfCells, sizeof(double));
    /*printf("29. calloc double *val - addRowsConstraints, ILP\n");*/
    if (val == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(ind);
        /*printf("28. free int *ind - addRowsConstraints, ILP\n");*/
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
                /*printf("28. free int *ind - addRowsConstraints, ILP\n");*/
				free(val);
                /*printf("29. free double *val - addRowsConstraints, ILP\n");*/
				return -1;
            }
            cnt = 0;
        }
    }
    free(ind);
    /*printf("28. free int *ind - addRowsConstraints, ILP\n");*/
    free(val);
    /*printf("29. free double *val - addRowsConstraints, ILP\n");*/
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
    /*printf("30. calloc int *imd - addBlocksConstraints, ILP\n");*/
    if (ind == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    val = (double *)calloc(blockNumOfCells, sizeof(double));
    /*printf("31. calloc double *val - addBlocksConstraints, ILP\n");*/
    if (val == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(ind);
        /*printf("30. free int *imd - addBlocksConstraints, ILP\n");*/
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
                    /*printf("30. free int *imd - addBlocksConstraints, ILP\n");*/
                    free(val);
                    /*printf("31. free double *val - addBlocksConstraints, ILP\n");*/
                    return -1;
                }
                cnt = 0;
            }
        }
    }
    free(ind);
    /*printf("30. free int *imd - addBlocksConstraints, ILP\n");*/
    free(val);
    /*printf("31. free double *val - addBlocksConstraints, ILP\n");*/
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
    /*double objval=0;*/

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
	/*print("1");*/
	*numOfVariables = updateVariables(puzzle, isILP, modelPtr, envPtr);
	/*print("2");*/

	*solPtr = (double *)calloc(*numOfVariables, sizeof(double));
    /*printf("32. calloc double *solPtr(sol) - findSolution, ILP\n");*/
	if (*solPtr == NULL) /* calloc failed */
	{
		printError(MemoryAllocFailed, NULL, 0, 0);
		exit(0);
	}
	/*print("3");*/
	/* update the model - to integrate new variables */
	error = GRBupdatemodel(model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}
	/*print("4");*/
	/* add constraints to model */
	if (addCellsConstraints(puzzle, model, env) == -1)
	{
		success = -1;
		goto END;
	}
	/*print("5");*/
	if (addRowsConstraints(puzzle, model, env) == -1)
	{
		success = -1;
		goto END;
	}
	/*print("6");*/
	if (addColsConstraints(puzzle, model, env) == -1)
	{
		success = -1;
		goto END;
	}
	/*print("7");*/
	if (addBlocksConstraints(puzzle, model, env) == -1)
	{
		success = -1;
		goto END;
	}
	/*print("8");*/
	/* Optimize model - need to call this before calculation */
	error = GRBoptimize(model);
	if (error) {
		printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}
	/*print("9");*/
	/* Write model to 'mip1.lp' - this is not necessary but very helpful */
	error = GRBwrite(model, "mip1.lp");
	if (error) {
		printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}
	/*print("10");*/
	/* Get solution information */
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}
	/*print("11");*/
	/* get the objective -- the optimal result of the function */
	/* not necessary but may be harmful to gurobi using */
	/*error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
	if (error) {
		printf("ERROR %d GRBgetdblattr(): %s\n", error, GRBgeterrormsg(env));
		return false;
	}*/

	/* print results */
	/*printf("\nOptimization complete\n");*/

	/* solution found */
	if (optimstatus == GRB_OPTIMAL) {
		success = 1;
		/*printf("Optimal objective: %.4e\n", objval);*/
	}
	/* no solution found */
	else if (optimstatus == GRB_INF_OR_UNBD) {
		success = 0;
		/*printf("Model is infeasible or unbounded\n");*/
		goto END;
	}
	/* error or calculation stopped */
	else {
		/*printf("Optimization was stopped early\n");*/
		success = -1;
		goto END;
	}

	/*print("beginning of if (success)");*/
	/* get the solution - the assignment to each variable */
	/* 3-- number of variables, the size of "sol" should match */
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, *numOfVariables, *solPtr);
	if (error) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
		success = -1;
		goto END;
	}
    /*printf("sol=%d\n", *solPtr==NULL?0:1);
    printf("sol[0]=%f\n", *solPtr[0]);
	print("end of if (success)");*/

	/*print("12");*/

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
	/*print("in ILPSolvable");*/
	success = findSolution(puzzle, true, &numOfVariables, &sol);
	/*print("after findSolution");*/
	if (sol!=NULL)
    {
        free(sol);
        /*printf("32. free double *solPtr(sol) - ILPSolvable, ILP\n");*/
    }
		
	freeVariables(puzzle->blockNumOfCells);
	/*print("after freeVariables");*/
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
    {
        free(sol);
        /*printf("32. free double *solPtr(sol) - ILPSolver, ILP\n");*/
    }
	freeVariables(puzzle->blockNumOfCells);
	return puzzle;
}

Move* fillDblSolution(Puzzle *puzzle, double threshold, double *sol)
{
    Move *head = NULL;
    Move *m = NULL;
    int row, col, k, index;
    double cellSol;
    int blockNumOfCells = puzzle->blockNumOfCells;
    double *scores = (double *)calloc(blockNumOfCells, sizeof(double));
    /*printf("33. calloc double *scores - fillDblSolution, ILP\n");*/
    if (scores==NULL)/* calloc failed */
    {
    	printError(MemoryAllocFailed, NULL, 0, 0);
    	exit(0);
    }
    if(head==NULL) /*???*/
    {
        head = NULL;
    }

    /*print("---1");*/

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
            /*printf("before fillCellAccordingToProb cell <%d,%d>\n", col, row);*/
        	m = fillCellAccordingToProb(puzzle, col, row, scores);
        	/*printf("after fillCellAccordingToProb cell <%d,%d>\n", col, row);*/
        	concat(&head, &m);
        	/*print("after concat");*/
        }
    }
    /*print("---2");*/
    free(scores);
    /*printf("33. free double *scores - fillDblSolution, ILP\n");*/
    /*print("---3");*/
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
    /*print("a");*/
	success = findSolution(puzzle, false, &numOfVariables, &sol);
	/*print("b");*/
	if (success == 1)
	{
		/*print("c");*/
		head = fillDblSolution(puzzle, threshold, sol);
        if(head==NULL)
	    {
		    addToList(&head, 0, 0, 0, 0);
	    }
		/*print("d");*/
	}
	/*print("e");*/
	if(sol!=NULL)
    {
        free(sol);
        /*printf("32. free double *solPtr(sol) - LPSolver, ILP\n");*/
    }
	/*print("f");*/
	freeVariables(puzzle->blockNumOfCells);
	/*print("g");*/
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
	/*printf("in LPCellValues\n");*/
    success = findSolution(puzzle, false, &numOfVariables, &sol);
    /*printf("success=%d\n", success);*/
    /*printf("numOfVariables=%d\n", numOfVariables);*/
    /*printf("sol=%d\n", sol==NULL?0:1);*/
	if (success == 1)
	{
		for (k=1; k<puzzle->blockNumOfCells+1; k++)
        {
            /*printf("k=%d\n", k);*/
            index = variables[row-1][col-1][k-1];
            /*printf("variables[%d][%d][%d]=%d\n", row-1, col-1, k-1, index);*/
            if (index)
            {
                /*printf("index=%d\n", index);*/
                values[k-1] = sol[index-1];
                /*printf("values[%d]=%f\n", k-1, values[k-1]);*/
            }
        }
	}
	if(sol!=NULL)
    {
        free(sol);
        /*printf("32. free double *solPtr(sol) - LPCellValues, ILP\n");*/
    }
	freeVariables(puzzle->blockNumOfCells);
	return success;
}

int ILPCellSolver(Puzzle *puzzle, int col, int row)
{
	int numOfVariables = 0;
	double *sol = NULL, isSol=0.0;
	int success=0, k=0, index=0, cellSol=0;
	success = findSolution(puzzle, true, &numOfVariables, &sol);
    /*printf("success=%d\n", success);
    printf("numOfVariables=%d\n", numOfVariables);
    printf("sol=%d\n", sol==NULL?0:1);*/
	if (success == 1)
	{
		for (k=1; k<puzzle->blockNumOfCells+1; k++)
        {
            /*printf("k=%d\n", k);
            printf("variables[%d][%d][%d]=%d\n", row-1, col-1, k-1, variables[row-1][col-1][k-1]);*/
            if (variables[row-1][col-1][k-1] != 0)
            {
                index = variables[row-1][col-1][k-1] - 1;
                /*printf("index=%d\n", index);*/
                isSol = sol[index];
                /*printf("isSol=%f\n", isSol);*/
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
        /*printf("32. free double *solPtr(sol) - ILPCellSolver, ILP\n");*/
    }
	freeVariables(puzzle->blockNumOfCells);
	return cellSol;
}
