#include "Parser.h"

bool DetermineEdit(char* optional) {
	return (optional == NULL ? editNew() : editFile(optional));
}

/* return true iff value is legal value (i.e. 0/1)
 * no using of atoi because it returns 0 if conversion failed */
bool SetMarkErrors(char* value) {
	if (value != NULL) {
		if (strcmp(value, "1") == 0) {
			EnableMarkErrors();
			return true;
		} else if (strcmp(value, "0") == 0) {
			DisableMarkErrors();
			return true;
		}
	}
	return false;
}

/*
 * Check whether numOfFixed is between minNum and maxNum
 * Return true if it is
 */
bool isNumInRange(int num, int minNum, int maxNum) {
	return (minNum <= num && num <= maxNum);
}

/*
 * this enum and printErrorAndExit can be moved to another header file for using along with Game.h
 */
typedef enum ErrorMsg {
	InvalidCommand, IndexOutOfBounds, TooLongInput, FunctionFailed, MemoryAllocFailed
} ErrorMsg;

void printErrorAndExit(ErrorMsg err, char* param1, int bound1, int bound2) {
	printf("Error: ");
	if (err==InvalidCommand)
		printf("Invalid Command");
	else if (err==IndexOutOfBounds)
		printf("Parameter Out Of Bounds. Bounds: %d-%d", bound1, bound2);
	else if(err==TooLongInput)
		printf("Max Number Of Characters: %d", MAX_INPUT_CHARS);
	else if (err==FunctionFailed)
		printf("Function %s Has Failed", param1);
	else if (err==MemoryAllocFailed)
		printf("Memory Allocation Failed");
	else
		printf("No Data About Error Found");
	printf("\n");
	exit(0);
}

Mode getCommand(Mode mode) {
	char input[MAX_INPUT_CHARS];
	char *fgetsRetVal, *token;
	int x, y, z, numOfSuccessfulScan;
	double xDouble;
	char *p = input;

	/* handle user input */
	/* notes:
	 * how to validate that input is 256 chars at most?
	 * how to handle whitespaces?
	 */
	fflush(stdin); /* not sure it's necessary */
	fgetsRetVal = fgets(input, sizeof(input), stdin);

	for (; *p; ++p)
		*p = tolower(*p); /* lowercase input */

	if (fgetsRetVal != NULL) {
		token = strtok(input, " \t\r\n");
	} else {
		if (feof(stdin)) /* treat EOF like "exit" */
			token = "exit";
		else if (ferror(stdin)) { /* treat error in user input, change later */
			printErrorAndExit(FunctionFailed, "fgets", 0, 0);
		}
	}
	if (token == NULL) /* no tokens in input, maybe whitespaces */
		exit(0); /* change later */

	/* call appropriate function */
	/* here "command=NULL" means invalid parameters/unavailable command */
	if (strcmp(token, "solve") == 0) {
		if (solve(strtok(NULL, " \t\r\n"))) /* assumption: change to new game iff new game loading succeeded */
			return Solve;
		printErrorAndExit(InvalidCommand,0,0,0);
	} else if (strcmp(token, "edit")==0) {
		DetermineEdit(strtok(NULL, " \t\r\n"));
		return Edit;
	} else if (strcmp(token, "mark_errors")==0) {
		if (mode == Solve && SetMarkErrors(strtok(NULL, " \t\r\n")) == true) /* next argument is not 0/1 */
			return mode;
		printErrorAndExit(InvalidCommand,0,0,0);
	} else if (strcmp(token, "print_board")==0) {
		if (mode == Solve || mode == Edit) {
			printBoard();
			return mode;
		}
		printErrorAndExit(InvalidCommand,0,0,0);
	} else if (strcmp(token, "set")==0) {
		if (mode == Solve || mode == Edit) {
			token = strtok(NULL, " \t\r\n");
			if (token != NULL) {
				numOfSuccessfulScan = sscanf(token, "%d", &x);
				if (numOfSuccessfulScan == 1 && isNumInRange(x, 1, Dim.m * Dim.n)) {
					token = strtok(NULL, " \t\r\n");
					if (token != NULL) {
						numOfSuccessfulScan = sscanf(token, "%d", &y);
						if (numOfSuccessfulScan == 1 && isNumInRange(y, 1, Dim.m * Dim.n)) {
							token = strtok(NULL, " \t\r\n");
							if (token != NULL) {
								numOfSuccessfulScan = sscanf(token, "%d", &z);
								if (numOfSuccessfulScan == 1) {
									if (set(x, y, z)) /* board solved */
										return Init;
									else
										return mode;
								}
							}
							printErrorAndExit(InvalidCommand,0,0,0);
						}
						printErrorAndExit(IndexOutOfBounds,0,1, Dim.m * Dim.n);
					}
					printErrorAndExit(InvalidCommand,0,0,0);
				}
				printErrorAndExit(IndexOutOfBounds,0,1, Dim.m * Dim.n);
			}
		}
		printErrorAndExit(InvalidCommand,0,0,0);
	} else if (strcmp(token, "validate")==0) {
		if (mode == Solve || mode == Edit) {
			validate();
			return mode;
		}
		printErrorAndExit(InvalidCommand,0,0,0);
	} else if (strcmp(token, "guess")==0) {
		if (mode == Solve) {
			token = strtok(NULL, " \t\r\n");
			if (token != NULL) {
				numOfSuccessfulScan = sscanf(token, "%lf", &xDouble);
				if (numOfSuccessfulScan == 1 && 0.0 <= xDouble && xDouble <= 1.0) {
					guess(xDouble);
					return mode;
				}
				printErrorAndExit(IndexOutOfBounds,0,0, 1);
			}
		}
		printErrorAndExit(InvalidCommand,0,0,0);
	} else if (strcmp(token, "generate")==0) {
		if (mode == Edit) {
			token = strtok(NULL, " \t\r\n");
			if (token != NULL) {
				numOfSuccessfulScan = sscanf(token, "%d", &x);
				if (numOfSuccessfulScan == 1 && isNumInRange(x, 0, Dim.m * Dim.n * Dim.m * Dim.n)) {
					token = strtok(NULL, " \t\r\n");
					if (token != NULL) {
						numOfSuccessfulScan = sscanf(token, "%d", &y);
						if (numOfSuccessfulScan == 1 && isNumInRange(y, 0, Dim.m * Dim.n * Dim.m * Dim.n)) {
							generate(x, y);
							return mode;
						}
						printErrorAndExit(IndexOutOfBounds,0,0, Dim.m * Dim.n * Dim.m * Dim.n);
					}
					printErrorAndExit(InvalidCommand,0,0,0);
				}
				printErrorAndExit(IndexOutOfBounds,0,0, Dim.m * Dim.n * Dim.m * Dim.n);
			}
		}
		printErrorAndExit(InvalidCommand,0,0,0);
	} else if (strcmp(token, "undo")==0) {
		if (mode == Solve || mode == Edit) {
			undo();
			return mode;
		}
		printErrorAndExit(InvalidCommand,0,0,0);
	} else if (strcmp(token, "redo")==0) {
		if (mode == Solve || mode == Edit) {
			redo();
			return mode;
		}
		printErrorAndExit(InvalidCommand,0,0,0);
	} else if (strcmp(token, "save")==0) {
		if (mode == Solve || mode == Edit) {
			save(strtok(NULL, " \t\r\n"));
			return mode;
		}
		printErrorAndExit(InvalidCommand,0,0,0);
	} else if (strcmp(token, "hint")==0) {
		if (mode == Solve) {
			token = strtok(NULL, " \t\r\n");
			if (token != NULL) {
				numOfSuccessfulScan = sscanf(token, "%d", &x);
				if (numOfSuccessfulScan == 1 && isNumInRange(x, 1, Dim.m * Dim.n)) {
					token = strtok(NULL, " \t\r\n");
					if (token != NULL) {
						numOfSuccessfulScan = sscanf(token, "%d", &y);
						if (numOfSuccessfulScan == 1 && isNumInRange(y, 1, Dim.m * Dim.n)) {
							hint(x, y);
							return mode;
						}
						printErrorAndExit(IndexOutOfBounds,0,1,Dim.m * Dim.n);
					}
					printErrorAndExit(InvalidCommand,0,0,0);
				}
				printErrorAndExit(IndexOutOfBounds,0,1,Dim.m * Dim.n);
			}
		}
		printErrorAndExit(InvalidCommand,0,0,0);
	} else if (strcmp(token, "guess_hint")==0) {
		if (mode == Solve) {
			token = strtok(NULL, " \t\r\n");
			if (token != NULL) {
				numOfSuccessfulScan = sscanf(token, "%d", &x);
				if (numOfSuccessfulScan == 1 && isNumInRange(x, 1, Dim.m * Dim.n)) {
					token = strtok(NULL, " \t\r\n");
					if (token != NULL) {
						numOfSuccessfulScan = sscanf(token, "%d", &y);
						if (numOfSuccessfulScan == 1 && isNumInRange(y, 1, Dim.m * Dim.n)) {
							guessHint(x, y);
							return mode;
						}
						printErrorAndExit(IndexOutOfBounds,0,1,Dim.m * Dim.n);
					}
					printErrorAndExit(InvalidCommand,0,0,0);
				}
				printErrorAndExit(IndexOutOfBounds,0,1,Dim.m * Dim.n);
			}
		}
		printErrorAndExit(InvalidCommand,0,0,0);
	} else if (strcmp(token, "num_solutions")==0) {
		if (mode == Solve || mode == Edit) {
			if (printf("%d", numSolution())==1)
				return mode;
			else
				printErrorAndExit(FunctionFailed,"printf",0,0);
		}
		printErrorAndExit(InvalidCommand,0,0,0);
	} else if (strcmp(token, "autofill")==0) {
		if (mode == Solve) {
			autoFill();
			return mode;
		}
		printErrorAndExit(InvalidCommand,0,0,0);
	} else if (strcmp(token, "reset")==0) {
		if (mode == Solve || mode == Edit) {
			reset();
			return mode;
		}
		printErrorAndExit(InvalidCommand,0,0,0);
	} else if (strcmp(token, "exit")==0) {
		Exit();
		return mode;
	} else
		printErrorAndExit(InvalidCommand,0,0,0);

	return mode;
}
