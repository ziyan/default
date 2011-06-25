/**
 * Base Class for Data
 */

#ifndef ERC_DATA_H
#define ERC_DATA_H
#include <cc++/thread.h>
#include <iostream>

namespace erc
{
	class Data
	{
	friend class DataPointer;
	friend std::istream &operator>>(std::istream &in, Data &data);
	friend std::ostream &operator<<(std::ostream &out, const Data &data);

	public:
		Data();
		virtual ~Data();
		virtual unsigned char getType() const = 0;
	private:
		ost::Mutex mutex;
		int refCounter;
	};
}

#endif
