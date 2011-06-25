#ifndef ERC_GPSDATA_H
#define ERC_GPSDATA_H

#include <iostream>
#include "../Data.h"

namespace erc
{
	class GpsData : public Data
	{
	public:
		double latitude; //latitude
		double longitude; //longitude
		double speed; //ground speed in m/s
		double altitude; //altitude in meters. void if Gps does not support 3D
		double climb; //vertical speed in m/s. void if Gps does not support 3D
		double x; //utm x
		double y; //utm y
		char zone[4]; //utm zone

		unsigned char getType() const;
		void updateUTM();
		double operator-(const GpsData &data) const;
		double operator/(const GpsData &data) const;
		friend std::istream &operator>>(std::istream &is, GpsData &data);
		friend std::ostream &operator<<(std::ostream &os, const GpsData &data);
	};
}

#endif
