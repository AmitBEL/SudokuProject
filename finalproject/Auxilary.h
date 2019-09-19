/*
 * This module contains function that several modules use them to avoid code duplication
 */
#ifndef AUXILARY_H_
#define AUXILARY_H_

#include "ErrorHandler.h"
#include "DoublyLinkedList.h"

/*
 * Mode types in the game: Init, Edit, Solve
 */
typedef enum Mode {Init, Edit, Solve} Mode;

/*
 * Cell contains info about cell
 * fixed - is a fixed cell, 0 - no, 1 - yes
 * value - the value
 * numOfCollitions - number of collisions with other illegal places with the same value
 */
typedef struct Cell {
	int fixed;
	int value;
	int numOfCollisions;
} Cell;

/*
 * Puzzle contains info about the current game:
 * board - 2d array of Cells
 * blockNumRow - num of rows in block
 * blockNumCol - num of cols in block
 * blockNumOfCells - num of cells in block
 * numOfCells - num of cells in board
 * numOfEmptyCells - num of empty cells in board
 * numOfErroneous - num of erroneous value in board
 */
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
