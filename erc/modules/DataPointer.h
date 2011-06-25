/**
 * Smart Pointer for Data
 */

#ifndef ERC_DATAPOINTER_H
#define ERC_DATAPOINTER_H
#include "Data.h"
#include <iostream>

namespace erc
{
	class DataPointer
	{
	friend std::istream &operator>>(std::istream &in, DataPointer &pointer);
	friend std::ostream &operator<<(std::ostream &out, const DataPointer &pointer);

	public:
		DataPointer(Data *data = 0);
		DataPointer(const DataPointer &other);
		~DataPointer();
		bool isNull() const;
		DataPointer &operator=(const DataPointer &other);
		bool operator==(const DataPointer &other) const;
		bool operator!=(const DataPointer &other) const;
		Data &operator*() const;
		Data *operator->() const;
	private:
		Data *data;
	};
}

#endif
