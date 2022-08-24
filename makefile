CC=gcc
AR=ar

all: voronoi

rmbin:
	rm voronoi

###################################00

voronoi: voronoi.o utils.o pngimage.o perlin.o
	$(CC) -o voronoi voronoi.o utils.o pngimage.o perlin.o -lpng -lm

voronoi.o: voronoi.c voronoi.h pngimage.h utils.h perlin.h
	$(CC) -c -O3 voronoi.c

utils.o: utils.c utils.h
	$(CC) -c -O3 utils.c

perlin.o: perlin.c perlin.h
	$(CC) -c -O3 perlin.c

pngimage.o: pngimage.c pngimage.h
	$(CC) -c -O3 pngimage.c

###################################00

clean:
	rm *.o
