CC = gcc
CFLAGS = -g -fsanitize=address
SRC = src/main.c src/token.c src/lexer.c src/parser.c
OBJS = build/main.o build/token.o build/lexer.o build/parser.o
EXECUTABLE = glyphc

main: main.o
	$(CC) -o $(EXECUTABLE) $(OBJS)

main.o: parser.o
	$(CC) -c src/main.c -o build/main.o

parser.o: lexer.o
	$(CC) -c src/parser.c -o build/parser.o

lexer.o: token.o
	$(CC) -c src/lexer.c -o build/lexer.o

token.o:
	$(CC) -c src/token.c -o build/token.o

clean:
	rm -rf build/*
