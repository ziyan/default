/*
 * UTM Conversions
 * This code was found at http://www.gpsy.com/gpsinfo/geotoutm/
 */
#include "LatLong-UTMconversion.h"
#include "UTMConversion.h"

using namespace erc;

void erc::LL2UTM(const GpsData &source,GpsData &dest)
{
	LLtoUTM(23, source.latitude, source.longitude, dest.x, dest.y, dest.zone);
}

void erc::UTM2LL(const GpsData &source,GpsData &dest)
{
	UTMtoLL(23, source.x, source.y, source.zone, dest.latitude, dest.longitude);
}

