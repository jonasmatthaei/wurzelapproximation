main: main.o reihe.o lookup.o performance.o
	gcc -o main main.o reihe.o lookup.o performance.o -lm

main.o: main.c reihe.h lookup.h performance.h
	gcc -c main.c

reihe.o: reihe.c reihe.h
	gcc -c reihe.c

lookup.o: lookup.c lookup.h
	gcc -c lookup.c

performance.o: performance.c performance.h reihe.h lookup.h main.h
	gcc -c performance.c

clear:
	rm main main.o reihe.o lookup.o performance.o
