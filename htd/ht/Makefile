# Copyright (C) Ziyan Zhou <zhou@ziyan.info> All right reserved.
all: htd

htd: htd.c glm.o
	gcc -o htd htd.c glm.o -lglut -lcv -lhighgui -lpthread

glm.o: glm.c
	gcc -c -o glm.o glm.c

clean:
	rm -f *~ *.o
