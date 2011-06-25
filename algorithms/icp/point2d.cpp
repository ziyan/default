#include "point2d.h"
#include <iostream>

using namespace std;

ostream &operator<<(ostream &os, const point2d_t &p)
{
	os << "(" << p.x << "," << p.y << ")";
	return os;
}

