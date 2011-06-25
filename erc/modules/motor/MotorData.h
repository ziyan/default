#ifndef ERC_MOTORDATA_H
#define ERC_MOTORDATA_H

#include <iostream>
#include "../Data.h"

namespace erc
{
	class MotorData : public Data
	{
	public:
		MotorData();
		~MotorData();
		unsigned char *value;
		unsigned char num;

		unsigned char getType() const;

		friend std::istream &operator>>(std::istream &is, MotorData &data);
		friend std::ostream &operator<<(std::ostream &os, const MotorData &data);
	};
}

#endif
