all: icp

%.o: %.cpp
	g++ -c -o $@ $<

icp: main.cpp tree2d.o ppm.o icp.o point2d.o
	g++ -o icp main.cpp tree2d.o ppm.o icp.o point2d.o

clean:
	find . -iname \*~ -exec rm -f {} \;
	find . -iname \*.o -exec rm -f {} \;

