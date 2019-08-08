#include <stdio.h>
#include <stdlib.h>
#include "Parser.h"
#include "SPBufferset.h"

int main() {
	int mode = 0; /* can't use enum Mode */

	SP_BUFF_SET();

	while (1) {
		mode = getCommand(mode);
	}

	return 0;
}
