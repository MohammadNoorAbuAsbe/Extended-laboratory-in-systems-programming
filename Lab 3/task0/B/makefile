all: B

B: assem.o
	gcc -g -m32 -Wall -o B  assem.o

assem.o: assem.s
	nasm -g -f elf -w+all -o assem.o assem.s

.PHONY:	clean

clean:  
	rm -f *.o B
