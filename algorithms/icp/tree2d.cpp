#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

#include "tree2d.h"

#define SQR(x) ((x)*(x))
#define DISTS(p1,p2) (SQR((p2).y-(p1).y)+SQR((p2).x-(p1).x))

using namespace std;

bool xorder(point2d_t p1, point2d_t p2) { return (p1.x < p2.x); }
bool yorder(point2d_t p1, point2d_t p2) { return (p1.y < p2.y); }

tree2d_t *tree2d_build(vector<point2d_t>::iterator begin, vector<point2d_t>::iterator end, axis_e axis, int depth)
{
	if (begin == end) return NULL;
	tree2d_t *tree = NULL;

	tree = (tree2d_t*)malloc(sizeof(tree2d_t));
	memset(tree, 0, sizeof(tree2d_t));

	// we sort the sub list so that we can build a balanced tree to speed up search
	sort(begin, end, axis == X ? xorder : yorder);
	vector<point2d_t>::iterator mid = begin + (int)((end - begin) / 2);
	tree->point = &(*mid);
	tree->axis = axis;

	if (depth > 0)
	{
		if (begin != mid)
			tree->subtree_less = tree2d_build(begin, mid, axis == X ? Y : X, depth - 1);
		if (end != mid + 1)
			tree->subtree_greater = tree2d_build(mid + 1, end, axis == X ? Y : X, depth - 1);
	}
	else
	{
		tree->begin = begin;
		tree->end = end;
	}
	return tree;
}

point2d_t *tree2d_search(const tree2d_t *tree, const point2d_t &point, float &gate)
{
	if (tree == NULL) return NULL;

	point2d_t *result = NULL, *r = NULL;

	float dist_to_mid = DISTS(*tree->point, point);
	if (dist_to_mid < gate) {
		gate = dist_to_mid;
		result = tree->point;
	}

	// if no subtree exists, we search through the list
	if (tree->subtree_less == NULL && tree->subtree_greater == NULL)
	{
		for (vector<point2d_t>::iterator i = tree->begin; i != tree->end; i++)
		{
			float dist_to_i = DISTS(*i, point);
			if (dist_to_i < gate) {
				gate = dist_to_i;
				result = &(*i);
			}
		}
	}
	else
	{
		if (tree->axis == X) {
			if (point.x < tree->point->x) {
				r = tree2d_search(tree->subtree_less, point, gate);
				if (r != NULL) result = r;
				if (SQR(point.x - tree->point->x) < gate) {
					r = tree2d_search(tree->subtree_greater, point, gate);
					if (r != NULL) result = r;
				}
			} else {
				r = tree2d_search(tree->subtree_greater, point, gate);
				if (r != NULL) result = r;
				if (SQR(point.x - tree->point->x) < gate) {
					r = tree2d_search(tree->subtree_less, point, gate);
					if (r != NULL) result = r;
				}
			}
		} else {
			if (point.y < tree->point->y) {
				r = tree2d_search(tree->subtree_less, point, gate);
				if (r != NULL) result = r;
				if (SQR(point.y - tree->point->y) < gate) {
					r = tree2d_search(tree->subtree_greater, point, gate);
					if (r != NULL) result = r;
				}
			} else {
				r = tree2d_search(tree->subtree_greater, point, gate);
				if (r != NULL) result = r;
				if (SQR(point.y - tree->point->y) < gate) {
					r = tree2d_search(tree->subtree_less, point, gate);
					if (r != NULL) result = r;
				}
			}
		}
	}
	return result;
}

void tree2d_free(tree2d_t **tree)
{
	if(tree && *tree)
	{
		if ((*tree)->subtree_less)
			tree2d_free(&((*tree)->subtree_less));
		if ((*tree)->subtree_greater)
			tree2d_free(&((*tree)->subtree_greater));
		free(*tree);
		*tree = NULL;
	}
}

