#ifndef ICP_PPM_H
#define ICP_PPM_H

#include "point2d.h"

#include <vector>

bool ppm_read(const char* file, std::vector<point2d_t> &points, int &width, int &height);

#endif
