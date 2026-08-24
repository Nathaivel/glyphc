CC = gcc
CFLAGS = -g -fsanitize=address -fno-omit-frame-pointer
SRC = src/main.c src/token.c src/lexer.c src/parser.c src/expressions.c src/statements.c
OBJS = build/main.o build/token.o build/lexer.o build/parser.o build/expressions.o build/statements.o
EXECUTABLE = glyphc

main: main.o
	$(CC) -o $(EXECUTABLE) $(OBJS)

main.o: parser.o
	$(CC) -c src/main.c -o build/main.o

parser.o: statements.o
	$(CC) -c src/parser.c -o build/parser.o

statements.o: expressions.o
	$(CC) -c src/statements.c -o build/statements.o

expressions.o: lexer.o
	$(CC) -c src/expressions.c -o build/expressions.o

lexer.o: token.o
	$(CC) -c src/lexer.c -o build/lexer.o

token.o:
	$(CC) -c src/token.c -o build/token.o

clean:
	rm -rf build/*
