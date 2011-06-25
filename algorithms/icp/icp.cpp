#include "icp.h"

#include <limits>
#include <cmath>
#include <iostream>

using namespace std;

pose_t icp_compute_relative_pose(const vector<const point2d_t*>& a, const vector<const point2d_t*>& b);
void icp_transform_pose(vector<point2d_t> &targets, const pose_t &pose);
void icp_accumulate_pose(pose_t &pose_acc, const pose_t &pose);

pose_t icp_align(const tree2d_t *tree, vector<point2d_t> &targets, pose_t pose, int iteration)
{
	pose_t pose_acc;
	pose_acc.p.x = 0;
	pose_acc.p.y = 0;
	pose_acc.yaw = 0;

	point2d_t *r;
	float dists;
	vector<const point2d_t*> a, b;
	icp_transform_pose(targets, pose);

	for (int i = 0 ; i < iteration; i ++)
	{
		a.clear();
		b.clear();
		for (vector<point2d_t>::const_iterator t = targets.begin(); t != targets.end(); t++)
		{
			dists = numeric_limits<float>::infinity();
			r = tree2d_search(tree, *t, dists);
			if (!r) continue;
			a.push_back(&(*t)); // target point
			b.push_back(r); // closest reference point
		}
		//cout << "found " << a.size() << endl;
		pose = icp_compute_relative_pose(a, b);
		//cout << "found pose " << pose << endl;

		icp_transform_pose(targets, pose);
		icp_accumulate_pose(pose_acc, pose);
		//cout << "accumulated pose " << pose_acc << endl;
	}
	return pose_acc;
}

pose_t icp_compute_relative_pose(const vector<const point2d_t*>& a, const vector<const point2d_t*>& b)
{
	int n = a.size();
	double x1 = 0.0, x2 = 0.0, y1 = 0.0, y2 = 0.0, xx = 0.0, yy = 0.0, xy = 0.0, yx = 0.0;

	for (int i=0; i<n; ++i)
	{
		const point2d_t& p1 = *a[i];
		const point2d_t& p2 = *b[i];

		x1 += p1.x;
		x2 += p2.x;
		y1 += p1.y;
		y2 += p2.y;
		xx += p1.x*p2.x;
		yy += p1.y*p2.y;
		xy += p1.x*p2.y;
		yx += p1.y*p2.x;
	}

	double N = (double)n;

	double Sxx = xx - x1*x2/N; // calculate S
	double Syy = yy - y1*y2/N;
	double Sxy = xy - x1*y2/N;
	double Syx = yx - y1*x2/N;

	double xm1 = x1/N; // calculate means
	double xm2 = x2/N;
	double ym1 = y1/N;
	double ym2 = y2/N;

	double yaw = atan2(Sxy-Syx, Sxx+Syy);

	pose_t pose; // calculate pose
	pose.p.x = xm2 - (xm1*cos(yaw) - ym1*sin(yaw));
	pose.p.y = ym2 - (xm1*sin(yaw) + ym1*cos(yaw));
	pose.yaw = yaw;
	return pose;
}

void icp_transform_pose(vector<point2d_t> &targets, const pose_t &pose)
{
	double c = cos(pose.yaw);
	double s = sin(pose.yaw);
	for (vector<point2d_t>::iterator t = targets.begin(); t != targets.end(); t++)
	{
		point2d_t &p = *t;

		p.x += pose.p.x;
		p.y += pose.p.y;
		double x = c * p.x - s * p.y;
		double y = s * p.x + c * p.y;
		p.x = x;
		p.y = y;
	}
}

void icp_accumulate_pose(pose_t &pose_acc, const pose_t &pose)
{
	double c = cos(pose.yaw);
	double s = sin(pose.yaw);

	pose_acc.p.x += pose.p.x;
	pose_acc.p.y += pose.p.y;
	double x = c * pose_acc.p.x - s * pose_acc.p.y;
	double y = s * pose_acc.p.x + c * pose_acc.p.y;
	pose_acc.p.x = x;
	pose_acc.p.y = y;
	pose_acc.yaw += pose.yaw;
	while (pose_acc.yaw >= M_PI) pose_acc.yaw -= 2.0 * M_PI;
	while (pose_acc.yaw < -M_PI) pose_acc.yaw += 2.0 * M_PI;
}

ostream &operator<<(ostream &os, const pose_t &p)
{
	os << "(" << p.p.x << "," << p.p.y << "," << p.yaw << ")";
	return os;
}


