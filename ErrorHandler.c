#include <stdio.h>
#include <stdlib.h>
#include "ErrorHandler.h"

/**
 * https://moodle.tau.ac.il/mod/forum/discuss.php?d=88531
 * need to change:
 * 1. exit only when cannot recover from error (e.g. memory allocation)
 * 2. add error about number of parameters
 */
void printError(ErrorMsg err, char* param1, int bound1, int bound2) {
	printf("Error: ");
	if (err == InvalidCommand)
		printf("Invalid Command");
	else if (err == IndexOutOfBounds)
		printf("Parameter Out Of Bounds. Bounds: %d-%d", bound1, bound2);
	else if (err == TooLongInput)
		printf("Max Number Of Characters: %d", MAX_INPUT_CHARS);
	else if (err == FunctionFailed)
		printf("Function %s Has Failed", param1);
	else if (err == MemoryAllocFailed) {
		printf("Memory Allocation Failed");
	} else if (err == ReadingFileFailed)
		printf("Reading File Failed");
	else if (err == WritingFileFailed)
		printf("Writing File Failed");
	else if (err == Erroneous)
		printf("Board Is Erroneous");
	else if (err == Validate)
		printf("Board Has No Solution");
	else if (err == WrongNumOfParams)
		printf("Wrong Number Of Parameters, The Correct Number Of Parameters Is %d", bound1);
	else if (err == WrongNumOfParamsBounds)
		printf("Wrong Number Of Parameters, The Correct Number Of Parameters Is %d or %d", bound1, bound2);
	else if (err == CommandFailed)
		printf("Command Failed");
	else if (err == WrongMode)
		printf("The Command Is Available Only In Mode(s) %s", param1);
	else if (err == ParamOutOfBounds)
		printf("Parameter %s Out Of Bounds. Bounds: %d-%d", param1, bound1, bound2);
	else if (err == ParamIsNotNum)
		printf("Parameter %s Must Be A Number", param1);
	else if (err == EmptyCellsParamRange)
		printf("Number Of Empty Cells For Parameter %s: %d-%d", param1, bound1, bound2);
	else if (err == GenerationFailed)
		printf("Puzzle Generator Has Failed");
	else if (err == NoMoreUndo)
		printf("No Moves To Undo Left");
	else if (err == NoMoreRedo)
		printf("No Moves To Redo Left");
	else if (err == FixedCell)
		printf("Cell Is Fixed");
	else if (err == CellHasValue)
		printf("Cell Has Value");
	else
		printf("No Data About Error Found");
	printf("\n");
}
