/**
 * Line Representation
 */

#include "Line.h"
#include "define.h"
#include <cmath>

using namespace std;
using namespace erc;

istream &erc::operator>>(istream &is, Line &line)
{
	Point p1,p2;
	is >> p1 >> p2;

	if(!is) return is;
	line.p1 = p1;
	line.p2 = p2;
	return is;
}
ostream &erc::operator<<(ostream &os, const Line &line)
{
	os << line.p1 << line.p2;
	return os;
}

double Line::length() const
{
	return p1-p2;
}

double Line::theta() const
{
	short int dy = p1.y - p2.y;
	short int dx = p2.x - p1.x;
	if(dy<0) 
	{
		dy = -dy;
		dx = -dx;
	}
	//horizontal line
	if(dy==0)
	{
		if(dx > 0) return 90.0;
		if(dx < 0) return -90.0;
		return 0.0;
	}	
	return RAD_TO_DEG(atan((double)dx/(double)dy));
}

