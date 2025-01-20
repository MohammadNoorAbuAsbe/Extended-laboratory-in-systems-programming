all: myshell

myshell: LineParser.o myShell.o looper
	gcc -g -m32 -Wall -o myshell LineParser.o myShell.o

myShell.o: myShell.c
	gcc -g -m32 -Wall -c -o myShell.o myShell.c

LineParser.o: LineParser.c
	gcc -g -m32 -Wall -c -o LineParser.o LineParser.c

looper: looper.c
	gcc -m32 -Wall -o looper looper.c

.PHONY: clean
clean:
	rm -f *.o myshell
