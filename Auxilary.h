#ifndef AUXILARY_H_
#define AUXILARY_H_

typedef enum {INIT, EDIT, SOLVE} Mode;

typedef struct {
	int fixed;
	int value;
	int numOfCollisions;
} Cell;

typedef struct Move {
	int x;
	int y;
	int oldValue;
	struct Move *nextMove;
} Move;

typedef struct Moves {
	Move *move;
	struct Moves *prevMoves;
	struct Moves *nextMoves;
} Moves;

typedef struct {
	Cell **board;
	int blockNumRow;
	int blockNumCol;
	int blockNumOfCells;
	int numOfCells;
	int numOfEmptyCells;
	int numOfErroneous;
} Puzzle;

#endif
