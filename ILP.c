#include "ILP.h"

/* private functions declaration */
void initVariables(int blockNumOfCells);

void freeVariables(int blockNumOfCells);

int updateVariables(Puzzle *puzzle, bool integer, GRBmodel model, GRBenv *env);

int addCellsConstraints(Puzzle *puzzle, GRBmodel model, GRBenv *env);

int addRowsConstraints(Puzzle *puzzle, GRBmodel model, GRBenv *env);

int addColsConstraints(Puzzle *puzzle, GRBmodel model, GRBenv *env);

int addBlocksConstraints(Puzzle *puzzle, GRBmodel model, GRBenv *env);

int addConstraints(Puzzle *puzzle, GRBmodel model, GRBenv *env);

bool findSolution(Puzzle *puzzle, bool integer, int *numOfVariables, double *sol);

void fillIntSolution(Puzzle *puzzle, double *sol, Mode mode);

Move *fillCellAccordingToProb(Puzzle *puzzle, int x, int y, double *values);

Move *fillThresholdSolution(Puzzle *puzzle, double *sol, double threshold);

int createEnvironment(GRBenv **env, char *logFileName, GRBenv *env);

int createModel(GRBenv *env, GRBmodel *model, char *modelName, GRBenv *env);

int addVars(GRBmodel *model, GRBenv *env, int numOfVarsToAdd, double *obj, char *vtype, char GRB_VTYPE);

int setIntAttr(GRBmodel *model, GRBenv *env);

int updateModel(GRBmodel *model, GRBenv *env);

int addConstraint(GRBmodel model, int numOfVars, int *ind, double *val, char *consName);

int optimize(GRBmodel *model, GRBenv *env);

int write(GRBmodel model, char *lpFileName, GRBenv *env);

int getIntAttr(GRBmodel *model, int *optimstatus, GRBenv *env);

int getDblAttr(GRBmodel *model, double *objval, GRBenv *env);

int getDblAttrArray(GRBmodel *model, int numOfVariables, double *sol);

/*--------*/

int ***variables;

int createEnvironment(GRBenv **env, char *logFileName, GRBenv *env)
{
    int error = 0;
    error = GRBloadenv(env, logFileName);
    if (error)
    {
        printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    error = GRBsetintparam(*env, GRB_INT_PAR_LOGTOCONSOLE, 0);
    if (error)
    {
        printf("ERROR %d GRBsetintparam(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    return 0;
}

int createModel(GRBenv *env, GRBmodel *model, char *modelName, GRBenv *env)
{
    int error = 0;
    error = GRBnewmodel(env, &model, modelName, 0, NULL, NULL, NULL, NULL, NULL);
    if (error)
    {
        printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }
    return 0;
}

int addVars(GRBmodel *model, GRBenv *env, int numOfVarsToAdd, double *obj, char *vtype, char GRB_VTYPE)
{
    int error = 0;
    error = GRBaddvars(model, numOfVarsToAdd, 0, NULL, NULL, NULL, obj, NULL, 1, vtype, NULL);
    if (error)
    {
        printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }
    return 0;
}

int setIntAttr(GRBmodel *model, GRBenv *env)
{
    int error = 0;
    error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
    if (error)
    {
        printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }
    return 0;
}

int updateModel(GRBmodel *model, GRBenv *env)
{
    int error = 0;
    error = GRBupdatemodel(model);
    if (error)
    {
        printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }
    return 0;
}

int addConstraint(GRBmodel model, GRBenv *env, int numOfVars, int *ind, double *val, char *consName)
{
    int error = 0;
    error = GRBaddconstr(model, numOfVars, ind, val, GRB_LESS_EQUAL, 1, consName);
    if (error)
    {
        printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }
    return 0;
}

int optimize(GRBmodel *model, GRBenv *env)
{
    int error = 0;
    error = GRBoptimize(model);
    if (error) {
        printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }
    return 0;
}

int write(GRBmodel *model, char *lpFileName, GRBenv *env)
{
    int error = 0;
    error = GRBwrite(model, lpFileName);
    if (error) {
        printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }
    return 0;
}

int getIntAttr(GRBmodel *model, int *optimstatus, GRBenv *env)
{
    int error = 0;
    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
    if (error) {
        printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }
    return 0;
}

int getDblAttr(GRBmodel *model, double *objval, GRBenv *env)
{
    int error = 0;
    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, objval);
    if (error) {
        printf("ERROR %d GRBgettdblattr(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }
    return 0;
}

/* run ILP and return if there is a solution or not */
int ILPSolvable(Puzzle *puzzle)
{
    int numOfVariables;
    double *sol = NULL;
    bool success;
    success = findSolution(puzzle, true, &numOfVariables, sol);
    free(sol);
    freeVariables(puzzle->blockNumOfCells);
    return success;
}

/* run ILP and return cell <x,y> value */
int ILPCellSolver(Puzzle *puzzle, int x, int y)
{
    int numOfVariables, value = 0, index, k;
    double *sol = NULL;
    bool success;
    success = findSolution(puzzle, true, &numOfVariables, sol);
    if (success)
    {
        for (k = 1; k < puzzle->blockNumOfCells + 1; k++)
        {
            index = variables[x - 1][y - 1][k - 1];
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
Move *LPSolver(Puzzle *puzzle, double threshold /*, Mode mode*/)
{
    Move *head = NULL;
    int numOfVariables /*, value=0, index, k*/;
    double *sol = NULL;
    bool success;
    success = findSolution(puzzle, false, &numOfVariables, sol);
    if (success)
    {
        head = fillThresholdSolution(puzzle, sol, threshold);
    }
    free(sol);
    freeVariables(puzzle->blockNumOfCells);
    return head;
}

/* run ILP and fill puzzle with the solution */
Puzzle *ILPSolver(Puzzle *puzzle)
{
    int numOfVariables /*, value=0, index, k*/;
    double *sol = NULL;
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
double *LPCellValues(Puzzle *puzzle, double threshold, int x, int y, double *values)
{
    int numOfVariables, index, k;
    double cellSol;
    double *sol = NULL;
    bool success;
    success = findSolution(puzzle, false, &numOfVariables, sol);
    if (success)
    {
        for (k = 1; k < puzzle->blockNumOfCells + 1; k++)
        {
            index = variables[x - 1][y - 1][k - 1];
            if (index)
            {
                cellSol = sol[index - 1];
                if (cellSol >= threshold)
                {
                    values[k - 1] = cellSol;
                }
                else
                {
                    values[k - 1] = 0;
                }
            }
            else
            {
                values[k - 1] = 0;
            }
        }
    }
    free(sol);
    freeVariables(puzzle->blockNumOfCells);
    return values;
}

/* fiil cell <x,y> by randomly choosing a value according to
 * values[] which is the probability
 * if none of the options is ligal don't fill it */
Move *fillCellAccordingToProb(Puzzle *puzzle, int x, int y, double *valueVsScore)
{
    double sum = 0, randVal = ((rand() % 10000) / 10000.0);
    int i = 0, N = puzzle->blockNumOfCells;

    /* sum the scores */
    for (i = 0; i < N; i++)
        sum += valueVsScore[i];
    if (sum == 0)
        return NULL;

    /* change the score of each cell to its relative probability */
    for (i = 0; i < N; i++)
        valueVsScore[i] = (valueVsScore[i] / sum);

    /* choose value randomly */
    i = 0;
    while (i < N && sum <= randVal)
    {
        sum += valueVsScore[i];
        i++;
    }

    return setCell(puzzle, x, y, i, Solve); /* the returned value is random value */
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

int updateVariables(Puzzle *puzzle, bool integer, GRBmodel model, GRBenv *env)
{
    int i, j, k, cnt = 1;
    int blockNumOfCells = puzzle->blockNumOfCells;
    double *obj;
    int *values;
    char *vtype;
    Cell *cell;
    values = (int *)calloc(blockNumOfCells + 1, sizeof(int));
    if (values == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    obj = (double *)calloc(blockNumOfCells, sizeof(double));
    if (obj == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(values);
        exit(0);
    }
    vtype = (char *)calloc(blockNumOfCells, sizeof(char));
    if (vtype == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        free(values);
        free(obj);
        exit(0);
    }
    for (i = 1; i < puzzle->blockNumOfCells + 1; i++)
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

                        if (integer)
                        {
                            obj[cnt - 1] = 1;
                            vtype[cnt - 1] = GRB_BINARY;
                        }
                        else
                        {
                            obj[cnt - 1] = random();
                            vtype[cnt - 1] = GRB_CONTINUOUS;
                        }
                        cnt++;
                    }
                }
            }
        }
    }
    if (addVars(model, env, cnt - 1, obj, vtype, integer) == -1)
    {
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
    return (cnt - 1);
}

int addCellsConstraints(Puzzle *puzzle, GRBmodel model, GRBenv *env)
{
    int i, j, k, num = 0, cnt = 0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    char temp[9] = {0};
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
    for (i = 1; i < blockNumOfCells + 1; i++)
    {
        for (j = 1; j < blockNumOfCells + 1; j++)
        {
            for (k = 1; k < blockNumOfCells + 1; k++)
            {
                if (variables[i - 1][j - 1][k - 1])
                {
                    ind[cnt] = variables[i - 1][j - 1][k - 1] - 1;
                    val[cnt] = 1;
                    cnt++;
                }
            }
            sprintf(temp, "a%d", num);
            if (addConstraint(model, env, cnt, ind, val, temp) == -1)
                free(ind);
            free(val);
            return -1;
            num++;
            cnt = 0;
        }
    }
    free(ind);
    free(val);
    return 0;
}

int addRowsConstraints(Puzzle *puzzle, GRBmodel model, GRBenv *env)
{
    int i, j, k, num = 0, cnt = 0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    char temp[9] = {0};
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
    for (j = 1; j < blockNumOfCells + 1; j++)
    {
        for (k = 1; k < blockNumOfCells + 1; k++)
        {
            for (i = 1; i < blockNumOfCells + 1; i++)
            {
                if (variables[i - 1][j - 1][k - 1])
                {
                    ind[cnt] = variables[i - 1][j - 1][k - 1] - 1;
                    val[cnt] = 1;
                    cnt++;
                }
            }
            sprintf(temp, "b%d", num);
            if (addConstraint(model, env, cnt, ind, val, temp) == -1)
                free(ind);
            free(val);
            return -1;
            num++;
            cnt = 0;
        }
    }
    free(ind);
    free(val);
    return 0;
}

int addColsConstraints(Puzzle *puzzle, GRBmodel model, GRBenv *env)
{
    int i, j, k, num = 0, cnt = 0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    char temp[9] = {0};
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
    for (i = 1; i < blockNumOfCells + 1; i++)
    {
        for (k = 1; k < blockNumOfCells + 1; k++)
        {
            for (j = 1; j < blockNumOfCells + 1; j++)
            {
                if (variables[i - 1][j - 1][k - 1])
                {
                    ind[cnt] = variables[i - 1][j - 1][k - 1] - 1;
                    val[cnt] = 1;
                    cnt++;
                }
            }
            sprintf(temp, "c%d", num);
            if (addConstraint(model, env, cnt, ind, val, temp) == -1)
                free(ind);
            free(val);
            return -1;
            num++;
            cnt = 0;
        }
    }
    free(ind);
    free(val);
    return 0;
}

int addBlocksConstraints(Puzzle *puzzle, GRBmodel model, GRBenv *env)
{
    int i, j, k, n, m, num = 0, index, cnt = 0;
    int blockNumOfCells = puzzle->blockNumOfCells;
    int blockNumOfCols = puzzle->blockNumCol;
    int blockNumOfRows = puzzle->blockNumRow;
    char temp[9] = {0};
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
                for (i = 1; i < blockNumOfCols + 1; i++)
                {
                    for (j = 1; j < blockNumOfRows + 1; j++)
                    {
                        index = variables[(n * blockNumOfCols) + (i - 1)][(m * blockNumOfRows) + (j - 1)][k - 1];
                        if (index)
                        {
                            ind[cnt] = index - 1;
                            val[cnt] = 1;
                            cnt++;
                        }
                    }
                }
                sprintf(temp, "d%d", num);
                if (addConstraint(model, env, cnt, ind, val, temp) == -1)
                {
                    free(ind);
                    free(val);
                    return -1;
                }
                num++;
                cnt = 0;
            }
        }
    }
    free(ind);
    free(val);
    return 0;
}

int addConstraints(Puzzle *puzzle, GRBmodel model, GRBenv *env)
{
    if (addCellsConstraints(puzzle, model, env) == -1)
        return -1;
    if (addRowsConstraints(puzzle, model, env) == -1)
        return -1;
    if (addColsConstraints(puzzle, model, env) == -1)
        return -1;
    if (addBlocksConstraints(puzzle, model, env) == -1)
        return -1;
    return 0;
}

bool findSolution(Puzzle *puzzle, bool integer, int *numOfVariables, double *sol)
{
    GRBenv *env = NULL;
    GRBmodel *model = NULL;
    int optimstatus;
    bool success;
    double objval;
    bool success;
    if (createEnvironment(env, "logFileName.log", env) == -1)
        return false;
    if (createModel(env, model, "modelName", env) == -1)
        return false;
    initVariables(puzzle->blockNumOfCells);
    *numOfVariables = updateVariables(puzzle, integer, model, env);
    if (updateModel(model, env) == -1)
        return false;
    if (addConstraints(puzzle, model, env) == -1)
        return false;
    if (optimize(model, env) == -1)
        return false;
    if (write(model, "lpFileName.lp", env) == -1)
        return false;
    if (getIntAttr(model, &optimstatus, env) == -1)
        return false;
    if (getDblAttr(model, &objval, env) == -1)
        return false;

    sol = (double *)calloc(*numOfVariables, sizeof(double));
    if (sol == NULL) /* calloc failed */
    {
        printError(MemoryAllocFailed, NULL, 0, 0);
        exit(0);
    }
    if (getDblAttrArray(model, *numOfVariables, sol) == -1)
        return false;

    success = (optimstatus == GRB_OPTIMAL);
    GRBfreemodel(model);
    GRBfreeenv(env);
    return success;
}

void fillIntSolution(Puzzle *puzzle, double *sol, Mode mode)
{
    int i, j, k, index, cellSol;
    int blockNumOfCells = puzzle->blockNumOfCells;
    /*Cell *cell; - unused var*/
    for (i = 1; i < blockNumOfCells + 1; i++)
    {
        for (j = 1; j < blockNumOfCells + 1; j++)
        {
            for (k = 1; k < blockNumOfCells + 1; k++)
            {
                index = variables[i - 1][j - 1][k - 1];
                if (index)
                {
                    cellSol = sol[index - 1];
                    if (cellSol)
                    {
                        setCell(puzzle, i, j, k, mode);
                    }
                }
            }
        }
    }
}

Move *fillThresholdSolution(Puzzle *puzzle, double *sol, double threshold)
{
    Move *head = NULL;
    Move *m;
    int i, j, k, index;
    double cellSol;
    int blockNumOfCells = puzzle->blockNumOfCells;
    double *scores = (double *)calloc(blockNumOfCells, sizeof(double));
    Cell *cell;
    for (i = 1; i < blockNumOfCells + 1; i++)
    {
        for (j = 1; j < blockNumOfCells + 1; j++)
        {
            cell = getCell(puzzle, i, j);
            if (!(cell->value))
            {
                for (k = 1; k < blockNumOfCells + 1; k++)
                {
                    index = variables[i - 1][j - 1][k - 1];
                    if (index)
                    {
                        cellSol = sol[index - 1];
                        if (cellSol >= threshold)
                        {
                            scores[k - 1] = cellSol;
                        }
                        else
                        {
                            scores[k - 1] = 0;
                        }
                    }
                    else
                    {
                        scores[k - 1] = 0;
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
