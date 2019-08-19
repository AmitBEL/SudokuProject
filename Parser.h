#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Game.h"
/*#include "Generator.h"*/

Mode getCommand(Mode mode);

void undoAllSteps(Mode mode);

void addStep(Move *moves);

void resetStepsList();

bool undo(Mode mode);

bool redo(Mode mode);

Mode isBoardCompleted(Mode mode);

bool EditType(char* optional, Mode mode);

void UpdateMarkErrors(char* value);

#endif
