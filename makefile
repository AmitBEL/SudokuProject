CC = gcc
OBJS = main.o Parser.o Game.o Generator.o DoublyLinkedList.o SingleLinkedList.o ErrorHandler.o Solver.o ILP.o Stack.o Auxilary.o 

EXEC = sudoku-console
COMP_FLAG = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c Parser.h SPBufferset.h
	$(CC) $(COMP_FLAG) -c $*.c
Parser.o: Parser.c Parser.h
	$(CC) $(COMP_FLAG) -c $*.c
Game.o: Game.c Game.h
	$(CC) $(COMP_FLAG) -c $*.c
Generator.o: Generator.c Generator.h
	$(CC) $(COMP_FLAG) -c $*.c
DoublyLinkedList.o: DoublyLinkedList.c DoublyLinkedList.h
	$(CC) $(COMP_FLAG) -c $*.c
SingleLinkedList.o: SingleLinkedList.c SingleLinkedList.h
	$(CC) $(COMP_FLAG) -c $*.c
ErrorHandler.o: ErrorHandler.c ErrorHandler.h
	$(CC) $(COMP_FLAG) -c $*.c
Solver.o: Solver.c Solver.h
	$(CC) $(COMP_FLAG) -c $*.c
ILP.o: ILP.c ILP.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
Stack.o: Stack.c Stack.h
	$(CC) $(COMP_FLAG) -c $*.c
Auxilary.o: Auxilary.c Auxilary.h
	$(CC) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
