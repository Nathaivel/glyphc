CC = gcc
CFLAGS = -Wall -MMD _MP
SRC = src/main.c src/token.c src/lexer.c
OBJS = build/main.o build/token.o build/lexer.o
EXECUTABLE = glyphc

main: main.o
	$(CC) -o $(EXECUTABLE) $(OBJS)

main.o: lexer.o
	$(CC) -c src/main.c -o build/main.o

lexer.o: token.o
	$(CC) -c src/lexer.c -o build/lexer.o

token.o:
	$(CC) -c src/token.c -o build/token.o

clean:
	rm -rf build/*
