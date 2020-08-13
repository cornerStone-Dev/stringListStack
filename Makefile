
all: bin stringListStack.o

stringListStack.o: stringListStack.c stringListStack.h
	gcc -Os -march=native stringListStack.c -c -o stringListStack.o -Wall -Wextra
	size stringListStack.o

bin/stringListStackTest: stringListStackTest.c stringListStack.o
	gcc -O2 -march=native stringListStackTest.c -s \
	-o bin/stringListStackTest stringListStack.o -Wall -Wextra

bin:
	mkdir bin

test: bin/stringListStackTest
	time ./bin/stringListStackTest

clean:
	rm -f stringListStack.o
	rm -f bin/stringListStackTest
