#ifndef ERC_PARALLELPORTDATA_H
#define ERC_PARALLELPORTDATA_H

#include <iostream>
#include "../Data.h"

#define SWITCH_NUM

namespace erc
{
	class ParallelPortData : public Data
	{
	public:
		ParallelPortData();
		~ParallelPortData();
		bool data[8];
		bool status[5];
		unsigned char getType() const;

		friend std::istream &operator>>(std::istream &is, ParallelPortData &data);
		friend std::ostream &operator<<(std::ostream &os, const ParallelPortData &data);
	};
}

#endif
