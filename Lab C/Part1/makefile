all: pipe

pipe: mypipeline.o
	gcc -g -m32 -Wall -o pipe mypipeline.o

mypipeline.o: mypipeline.c
	gcc -g -m32 -Wall -c -o mypipeline.o mypipeline.c

.PHONY: clean
clean:
	rm -f *.o pipe
