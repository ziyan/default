/**
 * Point Representation
 */
#include "Point.h"
#include "define.h"
#include <math.h>

using namespace std;
using namespace erc;

istream &erc::operator>>(istream &is, Point &p)
{
	short int x,y;
	char l,h;
	//x1
	is.get(l);
	is.get(h);
	x = WORD_TO_SHORT(l,h);
	//x1
	is.get(l);
	is.get(h);
	y = WORD_TO_SHORT(l,h);

	if(!is) return is;
	p.x = x;
	p.y = y;
	return is;
}
ostream &erc::operator<<(ostream &os, const Point &p)
{
	os << SHORT_TO_WORD_LOW(p.x) << SHORT_TO_WORD_HIGH(p.x);
	os << SHORT_TO_WORD_LOW(p.y) << SHORT_TO_WORD_HIGH(p.y);
	return os;
}

double Point::operator-(const Point &other) const
{
    return hypot(other.x-x, other.y-y);
}
