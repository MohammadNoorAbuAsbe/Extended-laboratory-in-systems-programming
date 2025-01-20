all: B

B: assem.o util.o
	gcc -g -m32 -Wall -o B  assem.o util.o

assem.o: assem.s
	nasm -g -f elf32 -w+all -o assem.o assem.s

util.o: util.c util.h
	gcc -g -m32 -Wall -c -o util.o util.c

.PHONY: clean

clean:  
	rm -f *.o B