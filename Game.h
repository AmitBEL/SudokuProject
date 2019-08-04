#ifndef GAME_H_
#define GAME_H_

#include "Auxilary.h"

Cell **board;

Moves *movesList;

int mark_errors=0;

Cell* getCell(int x, int y);

void solve(char* filepath); // change void to boolean

void editNew(); // change void to boolean

void editFile(char* filepath); // change void to boolean

void load(char* filepath);

void markErrors(int mark);

void printBoard();

void set(int x, int y, int z); // change void to boolean

void updateCollisions(int x, int y, int z, int num);

void calcCollisions(int x, int y, int z);

void validate(); // change void to boolean

void guess(float threshold);

void undo();

void redo();

void save(char* filepath);

void hint(int x, int y);

void guessHint(int x, int y);

void numSolution();

void autoFill();

void reset();

void exit();

#endif
