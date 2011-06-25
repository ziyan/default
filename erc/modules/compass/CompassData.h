#ifndef ERC_COMPASSDATA_H
#define ERC_COMPASSDATA_H

#include <iostream>
#include "../Data.h"

namespace erc
{
	class CompassData : public Data
	{
	public:
		short int direction;

		unsigned char getType() const;

		friend std::istream &operator>>(std::istream &is, CompassData &data);
		friend std::ostream &operator<<(std::ostream &os, const CompassData &data);
	};
}

#endif
