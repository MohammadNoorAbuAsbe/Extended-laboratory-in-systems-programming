all: hexeditplus

hexeditplus: hexeditplus.o
	gcc -g -Wall -o hexeditplus hexeditplus.o 
hexeditplus.o : hexeditplus.c
	gcc -g -Wall -c -o hexeditplus.o hexeditplus.c

.PHONY: clean
clean:	
	rm -f *.o virus_detector output
