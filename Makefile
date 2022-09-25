all: build

build: PPM

PPM: PPM.o
	gcc -o PPM PPM.o -g -Wall -lm -std=c99

quadtree.o: quadtree.c
	gcc -c PPM.c -g -Wall -lm -std=c99

clean:
	rm -rf *.o PPM
