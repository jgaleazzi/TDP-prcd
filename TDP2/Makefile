CC=gcc
CFLAGS= -lm -Wall
EXEC=main

all: $(EXEC)

main:  main.c src/load.o src/model.o
	mpicc -o main main.c  src/load.o src/model.o $(CFLAGS)

load.o: src/load.c
	$(CC) -o src/load.o -c src/load.c $(CFLAGS)

model.o: src/model.c
	$(CC) -o src/model.o -c src/model.c $(CFLAGS)

clean:
	rm -rf src/*.o  $(EXEC)  *~


