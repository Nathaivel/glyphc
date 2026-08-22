CC = gcc
CFLAGS = -Wall -MMD _MP
SRC = src/main.c src/token.c
OBJS = build/main.o build/token.o
EXECUTABLE = compiler

main: main.o
	$(CC) -o $(EXECUTABLE) $(OBJS)

main.o: token.o
	$(CC) -c src/main.c -o build/main.o

token.o:
	$(CC) -c src/token.c -o build/token.o

clean:
	rm -rf build/*
