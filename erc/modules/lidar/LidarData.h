#ifndef ERC_LIDARDATA_H
#define ERC_LIDARDATA_H

#define LIDAR_ANGLE 180.0
#define LIDAR_PRECISION 0.5
#define LIDAR_DATA_LENGTH 361

#include <iostream>
#include <map>
#include "../Data.h"

namespace erc
{
	class LidarData : public Data
	{
	public:
		unsigned char getType() const;
		unsigned short int distance[LIDAR_DATA_LENGTH];
		//std::map<short int, unsigned short int> direction;  //pointing to the suitable angles
		unsigned short int min;

		friend std::istream &operator>>(std::istream &is, LidarData &data);
		friend std::ostream &operator<<(std::ostream &os, const LidarData &data);
	};
}

#endif
