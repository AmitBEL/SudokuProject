/*
 * Contains the main function that initialize the game
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Parser.h"
#include "SPBufferset.h"

int main() {
	Mode mode = Init;

	SP_BUFF_SET();
	srand(time(NULL));

	printf("Welcome sudoku game!");

	while (1) {
		mode = getCommand(mode);
	}

	return 0;
}
