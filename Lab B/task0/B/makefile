all: exec

exec:hexaPrint.o
	gcc -g -Wall -o hexaPrint hexaPrint.o

encoder.o:hexaPrint.c
	gcc -g -Wall -c -o hexaPrint.o hexaPrint.c 

clean:
	rm -f *.o hexaPrint