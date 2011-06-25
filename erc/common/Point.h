/**
 * Point Representation
 */
#ifndef ERC_POINT_H
#define ERC_POINT_H
#include <iostream>

namespace erc
{
	struct Point
	{
		short int x;
		short int y;

		double operator-(const Point &other) const;
		friend std::istream &operator>>(std::istream &is, Point &p);
		friend std::ostream &operator<<(std::ostream &os, const Point &p);
	};
}

#endif
