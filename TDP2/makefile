CFLAGS=-W -Wall -std=c11
LDFLAGS=-lm

exec: main
	./$<

main: genpy
	gcc $(CFLAGS) -o $@ *.c $(LDFLAGS)

genpy:
	python gen_part.py $(NB_PROCS) $(NB_PARTS)

clean:
	rm *.o main
