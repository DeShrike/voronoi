CC=gcc
AR=ar

all: voronoi

rmbin:
	rm voronoi

###################################00

voronoi: voronoi.o utils.o
	$(CC) -o voronoi voronoi.o utils.o -lpng

voronoi.o: voronoi.c voronoi.h utils.h
	$(CC) -c -O3 voronoi.c

utils.o: utils.c utils.h
	$(CC) -c -O3 utils.c

###################################00

clean:
	rm *.o
