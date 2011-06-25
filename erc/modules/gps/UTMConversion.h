#ifndef ERC_UTMCONVERSION_H
#define ERC_UTMCONVERSION_H

#include "GpsData.h"

namespace erc
{
	void LL2UTM(const GpsData &source,GpsData &dest);
	void UTM2LL(const GpsData &source,GpsData &dest);
}
#endif
