#ifndef ICP_POINT2D_H
#define ICP_POINT2D_H

#include <iostream>

typedef struct point2d {
	float x, y;
} point2d_t;

std::ostream &operator<<(std::ostream &os, const point2d_t &p);

#endif
