#ifndef ILP_H_
#define ILP_H_

#include "Auxilary.h"

/* run ILP and return if there is a solution or not */
int ILPSolvable(Puzzle *puzzle);

/* run ILP and return a solution */
Puzzle* ILPSolver(Puzzle *puzzle);

/* 
 * run LP and return a new puzzle such that
 * all empty un-fixed cell values with a score
 * of threshold or greater are filled.
 * if several values hold for the same cell the value
 * is randomly chosen according to the score as the probability.
 * fills only legal values
 */
Move* LPSolver(Puzzle *puzzle, double threshold);

/* 
 * return a list such that in index i there is the
 * probability that the value of cell is i+1 
 */
double* LPCellValues(Puzzle *puzzle, double threshold, int x, int y, double *values);

int ILPCellSolver(Puzzle *puzzle, int x, int y);

#endif
