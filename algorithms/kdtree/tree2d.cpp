#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>

#define SQR(x) ((x)*(x))
#define DISTS(p1,p2) (SQR((p2).y-(p1).y)+SQR((p2).x-(p1).x))

using namespace std;

typedef struct point2d {
	float x;
	float y;
} point2d_t;

typedef enum axis {
	x = 0, y
} axis_e;

typedef struct tree2d
{
	point2d_t *point;
	axis_e axis;
	vector<point2d_t>::iterator begin, end;
	struct tree2d *subtree_less, *subtree_greater;
} tree2d_t;

ostream &operator<<(ostream &os, const point2d_t &p)
{
	os << "(" << p.x << "," << p.y << ")";
	return os;
}

bool xorder(point2d_t p1, point2d_t p2) { return (p1.x < p2.x); }
bool yorder(point2d_t p1, point2d_t p2) { return (p1.y < p2.y); }

tree2d_t *tree2d_build(vector<point2d_t>::iterator begin, vector<point2d_t>::iterator end, axis_e axis, int depth)
{
	if (begin == end) return NULL;
	tree2d_t *tree = NULL;

	tree = (tree2d_t*)malloc(sizeof(tree2d_t));
	memset(tree, 0, sizeof(tree2d_t));

	sort(begin, end, axis == x ? xorder : yorder);
	vector<point2d_t>::iterator mid = begin + (int)((end - begin) / 2);
	tree->point = &(*mid);
	tree->axis = axis;

	if (depth > 0)
	{
		if (begin != mid)
			tree->subtree_less = tree2d_build(begin, mid, axis == x ? y : x, depth - 1);
		if (end != mid + 1)
			tree->subtree_greater = tree2d_build(mid + 1, end, axis == x ? y : x, depth - 1);
	}
	else
	{
		tree->begin = begin;
		tree->end = end;
	}
	return tree;
}

point2d_t *tree2d_search(tree2d_t *tree, const point2d_t &point, float &gate)
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
		if (tree->axis == x) {
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

int main(int argc, char ** argv)
{
	if (argc != 4) {
		cerr << "Usage: " << argv[0] << " <depth> <point-x> <point-y>" << endl;
		return -1;
	}

	vector<point2d_t> points;
	while(cin)
	{
		point2d_t p;
		cin >> p.x >> p.y;
		if (cin)
			points.push_back(p);
	}

	cout << "Building tree ..." << endl;
	tree2d_t *tree = tree2d_build(points.begin(), points.end(), x, atoi(argv[1]));

	point2d_t point;
	point.x = atof(argv[2]);
	point.y = atof(argv[3]);
	float gate = numeric_limits<float>::infinity();

	cout << "Finding nearest point for: " << point << endl;
	for (int i = 0; i< 1000; i++)
	{
		gate = numeric_limits<float>::infinity();
		tree2d_search(tree, point, gate);
	}
	gate = numeric_limits<float>::infinity();
	point2d_t *best = tree2d_search(tree, point, gate);
	if (best)
		cout << "Found: " << (*best) << " with distance = " << sqrt(gate) << endl;
	return 0;
}
