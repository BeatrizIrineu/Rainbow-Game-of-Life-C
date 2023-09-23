CC = gcc

CFLAGS = -Wall -Wextra

TARGET = program

all: pthreads_graphic openmp_graphic pthreads_normal openmp_normal clean


pthreads_normal:
	$(CC)  pthreads.c -o pthreads.out

openmp_normal:
	$(CC)  openmp.c -o openmp.out -fopenmp

pthreads_graphic: graphic_functions.o
	$(CC) graphic_functions.o pthreads_graphic.c -o pthreads_graphic.out -lGL -lGLEW -lglfw

openmp_graphic: graphic_functions.o
	$(CC) graphic_functions.o openmp_graphic.c -o openmp_graphic.out -lGL -lGLEW -lglfw -fopenmp

graphic_compilation.o: graphic_functions.c graphic_functions.h
					$(CC) -c graphic_functions.c

clean:
	rm -f graphic_functions.o

.PHONY: all clean