all: iterator

iterator: iterator.o
	gcc -g -m32 -Wall -o iterator iterator.o 
iterator.o : iterator.c
	gcc -g -m32 -Wall -c -o iterator.o iterator.c

.PHONY: clean
clean:	
	rm -f *.o iterator
