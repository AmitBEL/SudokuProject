#ifndef AUXILARY_H_
#define AUXILARY_H_

#include "ErrorHandler.h"
#include "DoublyLinkedList.h"

typedef enum Mode {Init, Edit, Solve} Mode;

typedef struct Cell {
	int fixed;
	int value;
	int numOfCollisions;
} Cell;

typedef struct Puzzle {
	Cell **board;
	int blockNumRow;
	int blockNumCol;
	int blockNumOfCells;
	int numOfCells;
	int numOfEmptyCells;
	int numOfErroneous;
} Puzzle;

bool isBoardErr(Puzzle *puzzle);

bool isNumInRange(int num, int minNum, int maxNum);

Cell* getCell(Puzzle *puzzle, int x, int y);

Move* setCell(Puzzle *puzzle, int x, int y, int z, Mode mode);

void updateCollisions(Puzzle *puzzle, int x, int y, int newValue);

int *numOfCellSol(Puzzle *puzzle, int x, int y, int *values);

void subCollision(Puzzle *puzzle, Cell *cell);

void addCollision(Puzzle *puzzle, Cell *cell);

void updateRowCollisions(Puzzle *puzzle, int x, int y, int newValue);

void updateColCollisions(Puzzle *puzzle, int x, int y, int newValue);

void updateBlockCollisions(Puzzle *puzzle, int x, int y, int newValue);

#endif
