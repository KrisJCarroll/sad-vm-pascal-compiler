CFLAGS=-Wall -g

all: run

run: pascal
	./pascal

pascal: parser.o lexer.o
	g++ $(CFLAGS) -o $@ $+ -lm

%.o: %.cpp parser.h
	g++ $(CFLAGS) -c -Wall -std=c++11 -o $@ $<

parser.cpp lexer.cpp: pascal.y pascal.l
	lex -o lexer.cpp pascal.l
	bison $< -o parser.cpp --defines=parser.h

clean: FORCE
	rm -f parser.* lexer.* *.o calc

FORCE:

packages:
	sudo apt-get install flex bison make g++