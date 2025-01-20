all: multi

multi: multi.o
	gcc -g -m32 -Wall -o multi  multi.o
multi.o: multi.s
	nasm -g -f elf -w+all -o multi.o multi.s

	
.PHONY: clean

clean:	
	rm -f *.o multi