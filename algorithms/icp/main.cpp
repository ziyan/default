#include "point2d.h"
#include "tree2d.h"
#include "ppm.h"
#include "icp.h"

#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv)
{
	if (argc != 5) {
		cerr << "Usage: " << argv[0] << " <depth-of-tree> <refrence> <target> <iterations>" << endl;
		return -1;
	}

	int reference_width, reference_height, target_width, target_height;
	vector<point2d_t> reference, target;

	// read reference scan
	cout << "Reading from reference scan " << argv[2] << " ... " << flush;
	if(!ppm_read(argv[2], reference, reference_width, reference_height)) {
		cout << "failed!" << endl;
		return -2;
	}
	cout << "done! (" << reference.size() << " points)" << endl;

	for(vector<point2d_t>::const_iterator i = reference.begin(); i != reference.end(); i++)
		cout << (*i) << " ";
	cout << endl;

	// read target scan
	cout << "Reading from target scan " << argv[3] << " ... " << flush;
	if(!ppm_read(argv[3], target, target_width, target_height)) {
		cout << "failed!" << endl;
		return -2;
	}
	cout << "done! (" << target.size() << " points)" << endl;

	// build tree for search
	cout << "Building 2D tree ... " << flush;
	tree2d_t *tree = tree2d_build(reference.begin(), reference.end(), X, atoi(argv[1]));
	if (!tree) {
		cout << "failed!" << endl;
		return -3;
	}
	cout << "done!" << endl;


	// find pose
	cout << "Finding pose ... " << flush;
	pose_t pose;
	pose.p.x = 0;
	pose.p.y = 0;
	pose.yaw = 0;
	pose = icp_align(tree, target, pose, atoi(argv[4]));
	cout << pose << endl;

	// free tree
	cout << "Freeing 2D tree ... " << flush;
	tree2d_free(&tree);
	if (tree) {
		cout << "failed!" << endl;
		return -4;
	}
	cout << "done!" << endl;

	// output aligned points
	for(vector<point2d_t>::const_iterator i = target.begin(); i != target.end(); i++)
                cout << (*i) << " ";
        cout << endl;

	return 0;
}
