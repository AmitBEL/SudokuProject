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
	int value;
	struct Move *nextMove;
} Move;

typedef struct Moves {
	Move *move;
	struct Moves *prevMoves;
	struct Moves *nextMoves;
} Moves;

#endif
