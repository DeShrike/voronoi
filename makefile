CC=gcc
AR=ar

all: voronoi

rmbin:
	rm voronoi

###################################00

voronoi: voronoi.o
	$(CC) -o voronoi voronoi.o -lpng

voronoi.o: voronoi.c
	$(CC) -c -O3 voronoi.c

###################################00

clean:
	rm *.o
