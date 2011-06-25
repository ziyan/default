CC = gcc
LIB = libxbee.so.1.0.0
LIB_NAME = libxbee.so.1
LIB_DIR = .lib

all: $(LIB_DIR)/$(LIB)
$(LIB_DIR)/$(LIB): xbee/xbee.o
	mkdir -p $(LIB_DIR)
	$(CC) -shared -Wl,-soname,$(LIB_NAME) -o $(LIB_DIR)/$(LIB) xbee/xbee.o -lc

%.o: %.c
	$(CC) -fPIC -g -c -Wall -I. -I./include/ $< -o $@

clean:
	rm -Rf $(LIB_DIR)
	find . -iname \*.o -exec rm -f {} \;
	find . -iname \*~ -exec rm -f {} \;
