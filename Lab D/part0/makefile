all: myfile

myfile: myfile.o
	gcc -m32 -o myfile myfile.o

myfile.o: myfile.s
	nasm -f elf32 -o myfile.o myfile.s

clean:
	rm -f myfile myfile.o