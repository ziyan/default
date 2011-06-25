/**
 * Line Representation
 */
#ifndef ERC_LINE_H
#define ERC_LINE_H
#include <iostream>
#include "Point.h"

namespace erc
{
	struct Line
	{
		Point p1;
		Point p2;

		double length() const;
		double theta() const;

		friend std::istream &operator>>(std::istream &is, Line &line);
		friend std::ostream &operator<<(std::ostream &os, const Line &line);
	};
}

#endif
