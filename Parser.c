#include "Parser.h"

Step *stepsList=NULL, *currentMove=NULL; /* currentMove=NULL iff (stepsList=NULL || no more steps left to undo) */

int mark_errors = 1, last_mark_errors = 1;

void undoAllSteps(Mode mode){
	bool makeUndo=true;
	while (makeUndo)
		makeUndo=undo(mode);


	if (currentMove==NULL)
		printf("currentMove=NULL\n");
	else
		printf("currentMove=<%d,%d>\n", currentMove->moves->x, currentMove->moves->y);
	printList(stepsList);
}

bool undo(Mode mode){
	Move *moves;
	if (stepsList==NULL) /* currentMove=NULL too, the list is empty */
		return false;
	else if (currentMove==NULL) /* user made undo to all steps */
		return false;
	else{
		moves = currentMove->moves;
		while (moves!=NULL)
		{
			set(moves->x, moves->y, moves->oldValue, mode);
			moves=moves->next;
		}
		/* check if currentMove is not the first */
		if (currentMove->prev!=NULL)
			currentMove=currentMove->prev;
		else
			currentMove=NULL;

		if (currentMove==NULL)
			printf("currentMove=NULL\n");
		else
			printf("currentMove=<%d,%d>\n", currentMove->moves->x, currentMove->moves->y);
		printList(stepsList);

		return true;
	}
}

bool redo(Mode mode) {
	Move *moves;
	if (stepsList==NULL) /* currentMove=NULL too, the list is empty */
		return false;
	else if (currentMove==NULL) /* user made undo to all steps */
	{
		moves=stepsList->moves;
		while(moves!=NULL)
		{
			set(moves->x,moves->y, moves->newValue, mode);
			moves=moves->next;
		}
		currentMove=stepsList;

		if (currentMove==NULL)
			printf("currentMove=NULL\n");
		else
			printf("currentMove=<%d,%d>\n", currentMove->moves->x, currentMove->moves->y);
		printList(stepsList);

		return true;
	}
	else if(currentMove->next!=NULL) /* currentMove is not the last step */
	{
		moves=currentMove->next->moves;
		while(moves!=NULL)
		{
			set(moves->x,moves->y, moves->newValue, mode);
			moves=moves->next;
		}
		currentMove=currentMove->next;

		if (currentMove==NULL)
			printf("currentMove=NULL\n");
		else
			printf("currentMove=<%d,%d>\n", currentMove->moves->x, currentMove->moves->y);
		printList(stepsList);

		return true;
	}
	else
		return false;
}

void resetStepsList(){
	currentMove = stepsList;
	deleteAllNextNodes(&stepsList, currentMove);
	deleteNode(&stepsList, currentMove);
	stepsList=NULL;
	currentMove=NULL;

	if (currentMove==NULL)
		printf("currentMove=NULL\n");
	else
		printf("currentMove=<%d,%d>\n", currentMove->moves->x, currentMove->moves->y);
	printList(stepsList);
}

void addStep(Move* moves) {
	if (stepsList==NULL) /* currentMove=NULL too, the list is empty */
	{
		addToDoublyList(&stepsList, moves);
		currentMove=stepsList;
	}
	else if (currentMove==NULL) /* user made undo to all steps */
	{
		deleteAllNextNodes(&stepsList, stepsList);
		deleteNode(&stepsList, stepsList);
		addToDoublyList(&stepsList, moves);
		currentMove=stepsList;
	}
	else /* stepsList!=NULL && currentMove!=NULL (currentMove is at the end of the list) */
	{
		deleteAllNextNodes(&stepsList, currentMove);
		addToDoublyList(&stepsList, moves);
		currentMove=currentMove->next;
	}

	if (currentMove==NULL)
		printf("currentMove=NULL\n");
	else
		printf("currentMove=<%d,%d>\n", currentMove->moves->x, currentMove->moves->y);
	printList(stepsList);
}

Mode isBoardCompleted(Mode mode)
{
	if (mode==Solve && isSolved()){ /* if board solved start a new game */
		printf("Puzzle solved successfully!\n");
		return Init;
	}
	else{
		if (mode==Solve && !numOfEmptyCells() && isErroneous())
			printf("Board is erroneous!\n");
		return mode;
	}
}

bool EditType(char* optional, Mode mode) {
	return (optional == NULL ? editNew() : editFile(optional, mode));
}

/*
 * pre-cond: mode==Solve && (value=="1" || value=="0")
 */
void UpdateMarkErrors(char* value) {
	if (strcmp(value, "1") == 0) {
		mark_errors=1;
	} else if (strcmp(value, "0") == 0) {
		mark_errors=0;
	}
}

Mode getCommand(Mode mode) {
	char input[MAX_INPUT_CHARS], ch;
	char delimiter[]=" \t\r";
	char *fgetsRetVal, *token, *param1, *param2, *param3, *param4;
	int x, y, z, numOfSuccessfulScan;
	double xDouble;
	Move *moves;

	/*
	 * handle user input:
	 * 1. read a command from the user, each line is command
	 * 2. command with more than 256 chars are invalid
	 * 3. consider EOF as "exit"
	 * 4. ignore empty command (also empty lines, because each line is command)
	 */

	if (mode==Init)
		printf("Welcome sudoku game!");

	printf("\nEnter a command:\n");
	fgetsRetVal = fgets(input, MAX_INPUT_CHARS, stdin);

	if (input[MAX_INPUT_CHARS-2]!='\n' && input[MAX_INPUT_CHARS-2]!='\0'){ /* ensure 1<=command-length<=256 chars */
		printError(TooLongInput, NULL, 0, 0);
		for (x=0;x<300 && (ch=getchar())!='\n';x++)
		{
			putchar(ch);
		}
		return mode;
	}

	/* trim \n of enter in the end of the user input */
	input[strlen(input)-1] = '\0';

	if (fgetsRetVal != NULL) {
		token = strtok(input, delimiter);
	} else {
		if (feof(stdin)) /* treat EOF like "exit" */
			token = "exit";
		else if (ferror(stdin)) { /* treat error in user input, change later */
			printError(FunctionFailed, "fgets", 0, 0);
			return mode;
		}
	}
	if (token == NULL) /* no tokens in input means white-spaces so ignore */
		return mode;

	/* call appropriate function */

	/*****************************
When several errors exist for the same command, follow this order:
1. Command name is correct (otherwise it is an invalid command)
2. Command has the correct number of parameters
3. Parameters are correct. Report any error of parameter 1 before parameter 2, etc.
   For each parameter:
      a. It is in the correct range for the command
      b. Its value is legal for the current board state
4. The board is valid for the command (e.g., the board is not erroneous for "autofill").
5. The command is executed successfully (e.g., "save" command fails writing to the file)
	 *****************************/

	param1=strtok(NULL, delimiter);
	param2=strtok(NULL, delimiter);
	param3=strtok(NULL, delimiter);
	param4=strtok(NULL, delimiter);

	/* check if (correct num of param +1)!=NULL to determine if there are too many parameters
	 * for example, if (correct num of param)=1 and (param1==NULL || (param1!=NULL && param2!=NULL)),
	 * it means that:
	 *     a. param1 has not assigned, thus num of params is 0
	 *     b. param1 has assigned before param2, thus num of params is at least 2
	 *        (because this is how strtok works. otherwise: param1!=NULL && param2==NULL, thus num of
	 *        params is 2)
	 */

	if (strcmp(token, "solve") == 0) {/*1*/
		if (param1==NULL || (param1!=NULL && param2!=NULL)){
			printError(WrongNumOfParams, NULL,1,0);
			return mode;
		}
		if (solve(param1, Solve)){ /* assumption: change to new game iff new game loading succeeded */
			mark_errors = last_mark_errors;
			resetStepsList(); /* moves to the first step and then removes all steps and moves */
			printBoard(mark_errors);
			return Solve;
		}
		/* for many failure options, solve function handles its own errors */
		return mode;
	} else if (strcmp(token, "edit")==0) {/*2*/
		if (param1!=NULL && param2!=NULL){ /* has 0/1 params */
			printError(WrongNumOfParamsBounds, NULL,0,1);
			return mode;
		}
		if (EditType(param1, Edit)){
			last_mark_errors=mark_errors;
			resetStepsList(); /* moves to the first step and then removes all steps and moves */
			printBoard(mark_errors);
			return Edit;
		}
		/* for many failure options, EditType function handles its own errors */
		return mode;
	} else if (strcmp(token, "mark_errors")==0) {/*3*/
		if (mode==Solve){
			if (param1 ==NULL || (param1!=NULL && param2!=NULL)){
				printError(WrongNumOfParams, NULL,1,0);
				return mode;
			}
			if (strcmp(param1, "1") == 0 || strcmp(param1, "0") == 0) {
				UpdateMarkErrors(param1);
				return mode;
			}
			printError(IndexOutOfBounds, NULL, 0, 1);
			return mode;
		}
		printError(WrongMode, "Solve", 0, 0);
		return mode;
	} else if (strcmp(token, "print_board")==0) {/*4*/
		if (mode == Solve || mode == Edit){
			if (param1!=NULL)
				printError(WrongNumOfParams, NULL,0,0);
			else
				printBoard(mark_errors);
		}else
			printError(WrongMode, "Solve/Edit", 0, 0);
		return mode;
	} else if (strcmp(token, "set")==0) {/*5*/
		if (mode == Solve || mode == Edit){
			if (param1!=NULL && param2!=NULL && param3!=NULL && param4==NULL){
				numOfSuccessfulScan = sscanf(param1, "%d", &x);
				if (numOfSuccessfulScan == 1 && isNumInRange(x, 1, getBlockNumOfCells())){
					numOfSuccessfulScan = sscanf(param2, "%d", &y);
					if (numOfSuccessfulScan == 1 && isNumInRange(y, 1, getBlockNumOfCells())){
						numOfSuccessfulScan = sscanf(param3, "%d", &z);
						if (numOfSuccessfulScan == 1 && isNumInRange(y, 0, getBlockNumOfCells())){
							moves = set(x, y, z, mode);
							if (moves!=NULL){ /* cell is not fixed */
								addStep(moves); /* addStep removes the steps from current move to the end and then updates current.nextStep to moves */
							}
							printBoard(mark_errors);
							return isBoardCompleted(mode);
						}
						else{
							printError(ParamOutOfBounds,"3",0, getBlockNumOfCells());
							return mode;
						}
					}
					else{
						printError(ParamOutOfBounds,"2",1, getBlockNumOfCells());
						return mode;
					}
				}
				else{
					printError(ParamOutOfBounds,"1",1, getBlockNumOfCells());
					return mode;
				}
			}
			else{
				printError(WrongNumOfParams, NULL,3,0);
				return mode;
			}
		}
		printError(WrongMode, "Solve/Edit", 0, 0);
		return mode;
	}else if (strcmp(token, "validate")==0){/*6*/
		if(mode==Solve || mode==Edit){
			if (param1==NULL){
				if (!isErroneous())
					validate(true);
				else
					printError(Erroneous, NULL,0,0);
				return mode;
			}else{
				printError(WrongNumOfParams, NULL,0,0);
				return mode;
			}
		}
		printError(WrongMode, "Solve/Edit", 0, 0);
		return mode;
	} else if (strcmp(token, "guess")==0) { /*7*/
		if (mode==Solve){
			if (param1!=NULL){
				numOfSuccessfulScan = sscanf(token, "%lf", &xDouble);
				if (numOfSuccessfulScan == 1 && 0.0 <= xDouble && xDouble <= 1.0){
					if (!isErroneous()){
						moves = guess(xDouble, mode);
						if (moves!=NULL){ /* could not guess any value */
							addStep(moves); /* addStep removes the steps from current move to the end and then updates current.nextStep to moves */
						}
						printBoard(mark_errors);
						return mode;
					}
					else{
						printError(Erroneous, NULL,0,0);
						return mode;
					}
				}
				else{
					printError(IndexOutOfBounds,0,0, 1);
					return mode;
				}
			}
			else{
				printError(WrongNumOfParams, NULL,1,0);
				return mode;
			}
		}
		printError(WrongMode, "Solve", 0, 0);
		return mode;
	} else if (strcmp(token, "generate")==0) { /*8*/
		if (mode==Edit) {
			if (param1!=NULL && param2!=NULL && param3==NULL) {
				numOfSuccessfulScan = sscanf(param1, "%d", &x);
				if (numOfSuccessfulScan == 1 && isNumInRange(x, 0, numOfEmptyCells())){
					numOfSuccessfulScan = sscanf(param2, "%d", &y);
					if (numOfSuccessfulScan == 1 && isNumInRange(y, 0, getNumOfCells())){
						moves = generate(x, y);
						if (moves!=NULL){ /* could not generate board */
							addStep(moves); /* addStep removes the steps from current move to the end and then updates current.nextStep to moves */
							printBoard(mark_errors);
						}
						else
							printError(GenerationFailed, NULL, 0,0);
						return mode;
					}
					else{
						printError(ParamOutOfBounds, "2", 0, getNumOfCells());
						return mode;
					}
				}
				else{
					printError(EmptyCellsParamRange, "1", 0, numOfEmptyCells());
					return mode;
				}
			}
			else{
				printError(WrongNumOfParams, NULL,2,0);
				return mode;
			}
		}
		printError(WrongMode, "Edit", 0, 0);
		return mode;
	}else if (strcmp(token, "undo")==0){/*9*/
		if (mode==Solve || mode==Edit){
			if (param1==NULL){
				if (!undo(mode)){
					printError(NoMoreUndo, NULL,0,0);
				}
				printBoard(mark_errors);
				return mode;
			}
			else{
				printError(WrongNumOfParams, NULL,0,0);
				return mode;
			}
		}
		printError(WrongMode, "Solve/Edit", 0, 0);
		return mode;
	} else if (strcmp(token, "redo")==0) { /*10*/
		if (mode==Solve || mode==Edit){
			if (param1==NULL){
				if (!redo(mode)){
					printError(NoMoreRedo, NULL,0,0);
				}
				printBoard(mark_errors);
				return mode;
			}
			else{
				printError(WrongNumOfParams, NULL,0,0);
				return mode;
			}
		}
		printError(WrongMode, "Solve/Edit", 0, 0);
		return mode;
	} else if (strcmp(token, "save")==0) { /*11*/
		if (mode==Solve||mode==Edit){
			if (param1!=NULL && param2==NULL){
				if (mode==Edit){
					if (isErroneous()){
						printError(Erroneous, NULL, 0, 0);
						return mode;
					}
					if (!validate(false)){
						printError(Validate, NULL,0,0);
						return mode;
					}
				}
				if (save(param1, mode))
					return mode;
				else{
					printError(WritingFileFailed, NULL, 0, 0);
					return mode;
				}
			}
			else{
				printError(WrongNumOfParams, NULL,1,0);
				return mode;
			}
		}
		printError(WrongMode, "Solve/Edit", 0, 0);
		return mode;
	}else if (strcmp(token, "hint")==0){ /*12*/
		if (mode==Solve){
			if (param1!=NULL && param2!=NULL && param3==NULL) {
				numOfSuccessfulScan = sscanf(param1, "%d", &x);
				if (numOfSuccessfulScan == 1 && isNumInRange(x, 1, getNumOfCells())){
					numOfSuccessfulScan = sscanf(param2, "%d", &y);
					if (numOfSuccessfulScan == 1 && isNumInRange(y, 1, getNumOfCells())){
						if (isErroneous()){
							printError(Erroneous, NULL, 0, 0);
							return mode;
						}
						if (!validate(false)){
							printError(Validate, NULL,0,0);
							return mode;
						}
						hint(x, y);
						return mode;
					}
					else{
						printError(ParamOutOfBounds, "2", 1, getNumOfCells());
						return mode;
					}
				}
				else{
					printError(ParamOutOfBounds, "1", 1, getNumOfCells());
					return mode;
				}
			}
			else{
				printError(WrongNumOfParams, NULL,2,0);
				return mode;
			}
		}
		printError(WrongMode, "Solve", 0, 0);
		return mode;
	}else if (strcmp(token, "guess_hint")==0){/*13*/
		if (mode==Solve){
			if (param1!=NULL && param2!=NULL && param3==NULL) {
				numOfSuccessfulScan = sscanf(param1, "%d", &x);
				if (numOfSuccessfulScan == 1 && isNumInRange(x, 1, getNumOfCells())){
					numOfSuccessfulScan = sscanf(param2, "%d", &y);
					if (numOfSuccessfulScan == 1 && isNumInRange(y, 1, getNumOfCells())){
						if (isErroneous()){
							printError(Erroneous, NULL, 0, 0);
							return mode;
						}
						if (!validate(false)){
							printError(Validate, NULL,0,0);
							return mode;
						}
						guessHint(x, y);
						return mode;
					}
					else{
						printError(ParamOutOfBounds, "2", 1, getNumOfCells());
						return mode;
					}
				}
				else{
					printError(ParamOutOfBounds, "1", 1, getNumOfCells());
					return mode;
				}
			}
			else{
				printError(WrongNumOfParams, NULL,2,0);
				return mode;
			}
		}
		printError(WrongMode, "Solve", 0, 0);
		return mode;
	}else if (strcmp(token, "num_solutions")==0){/*14*/
		if (mode==Solve||mode==Edit){
			if (param1==NULL){
				if(!isErroneous()){
					numSolution();
					return mode;
				}
				else{
					printError(Erroneous,NULL,0,0);
					return mode;
				}
			}
			else{
				printError(WrongNumOfParams, NULL,0,0);
				return mode;
			}
		}
		printError(WrongMode, "Solve/Edit", 0, 0);
		return mode;
	}else if (strcmp(token, "autofill")==0){/*15*/
		if (mode==Solve){
			if (param1==NULL){
				if (!isErroneous()){
					moves = autoFill(mode);
					if (moves!=NULL){ /* could not fill any cell */
						addStep(moves); /* addStep removes the steps from current move to the end and then updates current.nextStep to moves */
					}
					printBoard(mark_errors);
					return isBoardCompleted(mode);
				}else{
					printError(Erroneous,NULL,0,0);
					return mode;
				}
			}else{
				printError(WrongNumOfParams, NULL,0,0);
				return mode;
			}
		}
		printError(WrongMode, "Solve", 0, 0);
		return mode;
	}else if (strcmp(token, "reset")==0){/*16*/
		if(mode==Solve||mode==Edit){
			if (param1==NULL){
				undoAllSteps(mode);
				printBoard(mark_errors);
				return mode;
			}else{
				printError(WrongNumOfParams, NULL,0,0);
				return mode;
			}
		}
		printError(WrongMode, "Solve/Edit", 0, 0);
		return mode;
	}else if (strcmp(token, "exit")==0){/*17*/
		if (param1==NULL){
			Exit();
			resetStepsList();
			return mode;
		}else{
			printError(WrongNumOfParams, NULL,0,0);
			return mode;
		}
	} else
		printError(InvalidCommand,0,0,0);

	return mode;
}
