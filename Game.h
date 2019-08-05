#ifndef GAME_H_
#define GAME_H_

#include "Auxilary.h"

struct {int m; int n;} Dim;

Cell* getCell(int x, int y);

bool solve(char* filepath);

bool editNew();

bool editFile(char* filepath);

void load(char* filepath);

void markErrors(int mark);

void printBoard();

bool set(int x, int y, int z);

void updateCollisions(int x, int y, int z, int num);

void calcCollisions(int x, int y, int z);

void validate(); /* change void to boolean */

void guess(float threshold);

void generate(int x, int y);

void undo();

void redo();

void save(char* filepath);

void hint(int x, int y);

void guessHint(int x, int y);

int numSolution();

void autoFill();

void reset();

void Exit();

void EnableMarkErrors();

void DisableMarkErrors();

#endif
