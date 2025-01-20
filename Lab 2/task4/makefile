all: exec

exec: mypipe.o
	gcc -g -m32 -Wall -o exec mypipe.o
mypipe.o : mypipe.c
	gcc -g -m32 -Wall -c -o mypipe.o mypipe.c

.PHONY: clean
clean:	
	rm -f *.o exec
