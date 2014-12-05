CC=mpicc
CFLAGS= -Wall
EXEC=main
#LIB= -Wl,-rpath=/usr/lib64 -Ilibgslcblas
LIB=/usr/lib64/libgslcblas.so.0 -lm
NB_PROC=4
BLOC_SIZE=2

run: compile
	/usr/lib64/openmpi/bin/mpiexec -n $(NB_PROC) ./$(EXEC)

compile: $(EXEC)

$(EXEC):  main.c load.o matrix.o
	$(CC) -o main main.c load.o matrix.o $(CFLAGS) $(LIB)

load.o: load.c
	$(CC) load.c -c $(CFLAGS)

matrix.o:
	$(CC) matrix.c -c $(CFLAGS)

gen:
	python matrix_gen.py $(NB_PROC) $(BLOC_SIZE)

clean:
	rm -rf *.o  $(EXEC)  *~


