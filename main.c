#include <stdio.h>
#include <stdlib.h>
#include "Parser.h"
#include "SPBufferset.h"

int main() {
	Mode mode = Init;

	SP_BUFF_SET();

	while (1) {
		mode = getCommand(mode);
	}

	return 0;
}
