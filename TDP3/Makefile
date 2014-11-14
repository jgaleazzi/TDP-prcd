CC=mpicc
CFLAGS= -lm -Wall
EXEC=main
LIB=-L/usr/lib64 -Ilibgslcblas

all: $(EXEC)

main:  main.c load.o
	$(CC) -o main main.c load.o $(LIB) $(CFLAGS)

load.o: load.c
	$(CC) load.c -c $(CFLAGS)
clean:
	rm -rf *.o  $(EXEC)  *~


