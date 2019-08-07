#ifndef GAME_H_
#define GAME_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Math.h>
#include "Auxilary.h"
#define MAX_FIRST_LINE_LENGTH 10

struct {int m; int n;} Dim;

Cell* getCell(int x, int y);

bool solve(char* filepath);

bool editNew();

bool editFile(char* filepath);

bool load(char* filepath);

void markErrors(int mark);

void printBoard();

bool set(int x, int y, int z);

void updateCollisions(int x, int y, int z, int num);

void calcCollisions(int x, int y, int z);

bool validate();

bool isErroneous();

void guess(float threshold);

int generate(int x, int y);

bool undo();

bool redo();

bool save(char* filepath, Mode mode);

void hint(int x, int y);

void guessHint(int x, int y);

int numSolution();

void autoFill();

void reset();

void Exit();

Cell* boardCellAccess(int x, int y);

void EnableMarkErrors();

void DisableMarkErrors();

int numOfEmptyCells();

#endif
