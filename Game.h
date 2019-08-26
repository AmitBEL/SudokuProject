#ifndef GAME_H_
#define GAME_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include <Math.h>*/
#include "Solver.h"
#include "Generator.h"

/* 
 * Game Module - Header
 * handle game commands
 */

bool solve(char* filepath, Mode mode);

bool editNew();

bool editFile(char* filepath, Mode mode);

void printBoard();

Move* set(int x, int y, int z, Mode mode);

bool validate(bool printResult);

bool isErroneous();

Move* guess(double threshold/*, Mode mode*/);

bool save(char* filepath, Mode mode);

void hint(int x, int y);

void guessHint(int x, int y);

void numSolution();

Move* autoFill();

void Exit();

int numOfEmptyCells();

int getBlockNumOfCells();

int getNumOfCells();

bool isSolved();

Move* generateBoard(int x, int y);

#endif
