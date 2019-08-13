#ifndef GAME_H_
#define GAME_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Math.h>
#include "Auxilary.h"
#define MAX_FIRST_LINE_LENGTH 10

Cell* getCell(int x, int y);

bool solve(char* filepath, Mode mode);

bool editNew();

bool editFile(char* filepath, Mode mode);

bool load(char* filepath, Mode mode);

void markErrors(int mark);

void printBoard();

Move** set(int x, int y, int z, Mode mode);

void updateCollisions(int x, int y, int z, int num);

void calcCollisions(int x, int y, int z);

bool validate();

bool isErroneous();

Move** guess(float threshold);

int generate(int x, int y);

bool undo();

bool redo();

bool save(char* filepath, Mode mode);

void hint(int x, int y);

void guessHint(int x, int y);

int numSolution();

Move** autoFill();

void reset();

void Exit();

Cell* boardCellAccess(int x, int y);

int numOfEmptyCells();

void createBoard(int blockNumOfRows, int blockNumOfCols);

int getNumOfRowInBlock();

int getNumOfColInBlock();

int getBlockNumOfCells();

int getNumOfCells();

#endif
