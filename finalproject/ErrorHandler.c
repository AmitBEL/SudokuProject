#include <stdio.h>
#include <stdlib.h>
#include "ErrorHandler.h"

/*
 * Print error message to the user considering the type of the error
 */
void printError(ErrorMsg err, char* param1, int bound1, int bound2) {
	printf("Error: ");
	if (err == InvalidCommand)
		printf("invalid command");
	else if (err == IndexOutOfBounds)
		printf("parameter out of bounds. Bounds: %d-%d", bound1, bound2);
	else if (err == TooLongInput)
		printf("max number of characters: %d", MAX_INPUT_CHARS-2);
	else if (err == FunctionFailed)
		printf("function %s has failed", param1);
	else if (err == MemoryAllocFailed) {
		printf("memory allocation failed");
	} else if (err == ReadingFileFailed)
		printf("reading file failed");
	else if (err == WritingFileFailed)
		printf("writing file failed");
	else if (err == Erroneous)
		printf("board is erroneous");
	else if (err == Validate)
		printf("board has no solution");
	else if (err == WrongNumOfParams)
		printf("wrong number of parameters. The correct number of parameters is %d", bound1);
	else if (err == WrongNumOfParamsBounds)
		printf("wrong number of parameters. The correct number of parameters is %d or %d", bound1, bound2);
	else if (err == CommandFailed)
		printf("command failed");
	else if (err == WrongMode)
		printf("the command is available only in mode(s) %s", param1);
	else if (err == ParamOutOfBounds)
		printf("parameter %s out of bounds. Bounds: %d-%d", param1, bound1, bound2);
	else if (err == ParamIsNotNum)
		printf("parameter %s must be a number", param1);
	else if (err == EmptyCellsParamRange)
		printf("number of empty cells for parameter %s: %d-%d", param1, bound1, bound2);
	else if (err == GenerationFailed)
		printf("puzzle generator has failed");
	else if (err == NoMoreUndo)
		printf("no moves to undo");
	else if (err == NoMoreRedo)
		printf("no moves to redo");
	else if (err == FixedCell)
		printf("cell <%d,%d> is fixed", bound1, bound2);
	else if (err == CellHasValue)
		printf("cell <%d,%d> already contains a value", bound1, bound2);
	else if (err == BigBoard)
		printf("board is too big. Max cell value must be at most 99");
	else if (err == DimNotPositive)
		printf("board dimensions must be positive");
	else if (err == IllegalBoard)
		printf("illegal board");
	else if (err == CommandDoesNotExists)
		printf("no such command exists");
	else
		printf("no data about error found");
	printf("\n");
}
