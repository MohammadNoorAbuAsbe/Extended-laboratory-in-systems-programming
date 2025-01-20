all: exec

exec:BubbleSort.o
	gcc -g -Wall -o BubbleSort BubbleSort.o

encoder.o:BubbleSort.c
	gcc -g -Wall -c -o BubbleSort.o BubbleSort.c 

clean:
	rm -f *.o BubbleSort