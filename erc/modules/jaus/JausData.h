#ifndef ERC_JAUSDATA_H
#define ERC_JAUSDATA_H

#include <iostream>
#include "../Data.h"

namespace erc
{
	class JausData : public Data
	{
	public:
		bool mobility;
		bool warning;
		
		unsigned char getType() const;
		
		friend std::istream &operator>>(std::istream &is, JausData &data);
		friend std::ostream &operator<<(std::ostream &os, const JausData &data);
	};
}


#endif /*ERC_JAUSDATA_H*/
