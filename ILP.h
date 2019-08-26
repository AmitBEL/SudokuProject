#ifndef ILP_H_
#define ILP_H_

#include <stdbool.h>
#include "Auxilary.h"
#include "gurobi_c.h"


/* run ILP and return if there is a solution or not */
int ILPSolvable(Puzzle *puzzle);

/* run ILP and return cell <x,y> value */
int ILPCellSolver(Puzzle *puzzle, int x, int y);

/* run ILP and fill puzzle with the solution */
Puzzle* ILPSolver(Puzzle *puzzle);

/* 
 * run LP and return a new puzzle such that
 * all empty un-fixed cell values with a score
 * of threshold or greater are filled.
 * if several values hold for the same cell the value
 * is randomly chosen according to the score as the probability.
 * fills only legal values
 */
Move* LPSolver(Puzzle *puzzle, double threshold/*, Mode mode*/);

/* 
 * return a list such that in index i there is the
 * probability that the value of cell is i+1 
 */
double* LPCellValues(Puzzle *puzzle, double threshold, int x, int y, double *values);

#endif
