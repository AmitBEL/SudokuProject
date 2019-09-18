/*
 * This module manages the generating of a new board
 */

#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <stdlib.h>
#include "ILP.h"

Move* generate(Puzzle *puzzle, int x, int y);

void fillRandPossibleValue(Puzzle *puzzle, int col, int row);

void copyBoard(Puzzle* source, int** dest);

void changePuzzle(Puzzle* puzzle, int** source);

#endif
