all: cvrecord
cvrecord: cvrecord.c
	gcc -ggdb -Wall -o cvrecord cvrecord.c -lcv -lhighgui -lcvaux
clean:
	rm -f *~
	rm -f *.o
	rm -f cvrecord
