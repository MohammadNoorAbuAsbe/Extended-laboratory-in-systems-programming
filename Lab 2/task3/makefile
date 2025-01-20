all: exec

exec: LineParser.o myShell.o
	gcc -g -m32 -Wall -o exec LineParser.o myShell.o
myShell.o : myShell.c
	gcc -g -m32 -Wall -c -o myShell.o myShell.c
LineParser.o : LineParser.c
	gcc -g -m32 -Wall -c -o LineParser.o LineParser.c

.PHONY: clean
clean:	
	rm -f *.o exec
