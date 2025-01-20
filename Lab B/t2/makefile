all: exec

exec:AntiVirus.o
	gcc -g -Wall -o AntiVirus AntiVirus.o

encoder.o:AntiVirus.c
	gcc -g -Wall -c -o AntiVirus.o AntiVirus.c 

clean:
	rm -f *.o AntiVirus