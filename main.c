#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Parser.h"
#include "SPBufferset.h"

int main() {
	Mode mode = Init;
	/*int i=0;*/
	SP_BUFF_SET();
	/*
	for (i=0;i<256;i++)
		printf("1");

	srand(time(NULL));
	 */
	while (1) {
		mode = getCommand(mode);
	}

	return 0;
}
