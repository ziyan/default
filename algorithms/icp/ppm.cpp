#include "ppm.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

bool ppm_read(const char* file, vector<point2d_t> &points, int &width, int &height)
{
	ifstream is(file);
	if (!is) return false;

	string version;
	is >> version;
	if (!is) return false;
	if (version != "P3") return false;

	char comment[1024];
	is.getline(comment, 1024);
	is.getline(comment, 1024); // not a bug
	if (!is) return false;

	is >> width >> height;
	if (!is) return false;

	int r, g, b;
	is >> r;
	if (!is) return false;
	if (r != 255) return false;

	for (int y = 0; y < height; y ++)
	{
		for (int x = 0; x < width; x++)
		{
			is >> r >> g >> b;
			if (!is) return false;
			if (r != 255 || g != 255 || b != 255) {
				point2d_t p;
				p.x = x;
				p.y = y;
				points.push_back(p);
			}
		}
	}
	return true;
}

