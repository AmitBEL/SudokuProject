/*
 * This module manages the errors to show
 */
#ifndef ERRORHANDLER_H_
#define ERRORHANDLER_H_
#define MAX_INPUT_CHARS 258 /* DON'T CHANGE IT, IT'S FOR THE INPUT ARRAY */

/*
 * ErrorMsg contains the types of possible errors
 */
typedef enum ErrorMsg {
	InvalidCommand, IndexOutOfBounds, TooLongInput, FunctionFailed, MemoryAllocFailed,
	ReadingFileFailed, WritingFileFailed, Erroneous, Validate, WrongNumOfParams, WrongNumOfParamsBounds,
	CommandFailed, WrongMode, ParamOutOfBounds, ParamIsNotNum, EmptyCellsParamRange,
	GenerationFailed, NoMoreUndo, NoMoreRedo, FixedCell, CellHasValue, BigBoard, IllegalBoard,
	DimNotPositive, CommandDoesNotExists
} ErrorMsg;

void printError(ErrorMsg err, char* param1, int bound1, int bound2);

#endif /* ERRORHANDLER_H_ */
