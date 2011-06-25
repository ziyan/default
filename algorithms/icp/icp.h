#ifndef ICP_ICP_H
#define ICP_ICP_H

#include "point2d.h"
#include "tree2d.h"

#include <iostream>

typedef struct pose {
	point2d_t p;
	float yaw;
} pose_t;
std::ostream &operator<<(std::ostream &os, const pose_t &p);

pose_t icp_align(const tree2d_t *tree, std::vector<point2d_t> &targets, pose_t guess, int iteration);

#endif
