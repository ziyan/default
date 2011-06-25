CC = gcc
LIB_DIR = .lib

all: test
test: test.c
	$(CC) -Wall -g -o test test.c ../xbee/xbee.o -I. -I../include/

clean:
	rm -f test
	find . -iname \*.o -exec rm -f {} \;
	find . -iname \*~ -exec rm -f {} \;
