all: loader.o

loader.o: loader.c start.o
	gcc -g -m32 -Wall -c -o loader.o loader.c 
	ld -o loader loader.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

start: start.o
	 gcc -g -m32 -Wall -o start start.o

start.o: start.s
	nasm -g -f elf -w+all -o start.o start.s  

.PHONY: clean
clean:	
	rm -f *.o loader