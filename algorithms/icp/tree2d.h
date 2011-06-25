#ifndef ICP_TREE2D_H
#define ICP_TREE2D_H

#include "point2d.h"
#include <vector>

typedef enum axis {
	X = 0, Y
} axis_e;

typedef struct tree2d {
	point2d_t *point;
	axis_e axis;
	std::vector<point2d_t>::iterator begin, end;
	struct tree2d *subtree_less, *subtree_greater;
} tree2d_t;

tree2d_t *tree2d_build(std::vector<point2d_t>::iterator begin, std::vector<point2d_t>::iterator end, axis_e axis, int depth);
point2d_t *tree2d_search(const tree2d_t *tree, const point2d_t &point, float &gate);
void tree2d_free(tree2d_t **tree);

#endif
