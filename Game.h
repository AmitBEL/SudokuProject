#ifndef GAME_H_
#define GAME_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Math.h>
#include "Auxilary.h"
#include "ILP.h"
#include "Solver.h"

Cell* getCell(int x, int y);

bool solve(char* filepath, Mode mode);

bool editNew();

bool editFile(char* filepath, Mode mode);

bool load(char* filepath, Mode mode);

void markErrors(int mark);

void printBoard();

Move* set(int x, int y, int z, Mode mode);

void updateCollisions(int x, int y, int newValue);

bool validate(bool printResult);

bool isErroneous();

bool hasErroneousFixedCells();

Move* guess(float threshold, Mode mode);
/*
Move* generate(int x, int y);

int undo();

int redo();
*/
bool save(char* filepath, Mode mode);

void hint(int x, int y);

void guessHint(int x, int y);

void numSolution();

Move* autoFill();

void Exit();

Cell* boardCellAccess(int x, int y);

int numOfEmptyCells();

void createBoard(int blockNumOfRows, int blockNumOfCols);

int getNumOfRowInBlock();

int getNumOfColInBlock();

int getBlockNumOfCells();

int getNumOfCells();

bool isSolved();

#endif
