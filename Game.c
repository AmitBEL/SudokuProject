#include <stdio.h>
#include <stdlib.h>
#include "Game.h"

Cell **board;

Moves *movesList;



bool fillBoard(FILE* fp, Mode mode) {
	int i, j, N=Dim.m*Dim.n;
	char str[21]={0}; /* max num is 2^64 and its length is 20 chars (+1 for \0 char) */
	Cell *cell;

	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			cell = &board[i][j];
			/*
			 * read every number as string.
			 * read the string as number and assign it as value (sscanf reads number with "." in the end as ".0" and thus reads the number as int).
			 * set as fixed if last char in string is ".".
			 */
			if (fscanf(fp, "%s", str)==1) {
				if (sscanf(str, "%d", &(cell->value))==1) {
					if (str[((int)strlen(str))-1]=='.' && mode==Solve)
						cell->fixed=1;
				}
				else
					return false;
			}
			else
				return false;
		}
	}
	return true;
}

/*
 * should detect erroneous values in board? (erroneous board can be saved in solve mode)
 * return true if assignment to board succeeded,
 * otherwise return false (failure in this function is treated as function failed, parameter 5 in parser.c)
 */
bool load(char* filepath, Mode mode) {
	FILE* fp;
	int m,n;
	char line[MAX_FIRST_LINE_LENGTH];


	fp = fopen(filepath, "r");
	if (fp == NULL)
		return false;

	fgets(line, MAX_FIRST_LINE_LENGTH, fp);
	if (sscanf(line, "%d %d", &m, &n)!=2)
		return false;
	Dim.m=m;
	Dim.n=n;

	createBoard(Dim.m, Dim.n);
	if (!fillBoard(fp, mode))
		return false;

	fclose(fp);

	return true;
}

bool save(char* filepath, Mode mode) {
	FILE *fp = fopen(filepath, "w");
	int N=Dim.m*Dim.n, i, j, charsToWrite;
	Cell *cell;

	if (fp == NULL){
		fclose(fp);
		return false;
	}
	if(fprintf(fp, "%d %d\n", Dim.m, Dim.n)!=4){
		fclose(fp);
		return false;
	}
	for(i=0;i<N;i++){
			for(j=0;j<N;j++){
				cell = &board[i][j];
				if (j>0){
					if (fprintf(fp, " ")!=1){
						fclose(fp);
						return false;
					}
				}
				if (cell->value==0)
					charsToWrite=1;
				else
					charsToWrite=(int)(floor(log10((double)(cell->value))))+1;
				if (fprintf(fp, "%d", cell->value)!=charsToWrite){
					fclose(fp);
					return false;
				}
				if (cell->fixed==1 || (mode==Edit && cell->value!=0)){
					if (fprintf(fp, ".")!=1){
						fclose(fp);
						return false;
					}
				}
			}
			if (fprintf(fp, "\n")!=1){
				fclose(fp);
				return false;
			}
	}

	if (fclose(fp)!=0)
		return false;

	return true;
}
