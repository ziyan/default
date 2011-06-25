CC=gcc
CFLAGS=-Wall
LDFLAGS=-lm
SOURCES=main.c perceptron.c connection.c layer.c ann.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=ann

all: $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	find . -iname \*.o -exec rm {} \;
	find . -iname \*~ -exec rm {} \;
