# Memory pool Makefile
All: Main.exe

Main.exe: Main.o MemPool.o
	gcc -o Main.exe Main.o MemPool.o

Main.o: MemPool.h Main.c
	gcc -std=c99 -c Main.c -o Main.o

MemPool.o: MemPool.h MemPool.c
	gcc -c MemPool.c -o MemPool.o

.PHONY: clean

clean:
	rm *.o