#include "LidarData.h"
#include "../../common/define.h"
#include "../ModulesDefine.h"

using namespace std;
using namespace erc;

unsigned char LidarData::getType() const
{
    return DATATYPE_LIDARDATA;
}

istream &erc::operator>>(istream &is, LidarData &data)
{
	short int distance[LIDAR_DATA_LENGTH];
	char l,h;
	for(int i=0;i<LIDAR_DATA_LENGTH;i++)
	{
		is.get(l);
		is.get(h);
		distance[i] = WORD_TO_SHORT(l,h);
	}
	if(!is) return is;
	copy(distance,distance+LIDAR_DATA_LENGTH,data.distance);
	return is;
}

ostream &erc::operator<<(ostream &os, const LidarData &data)
{
	for(int i=0;i<LIDAR_DATA_LENGTH;i++)
	{
		os << SHORT_TO_WORD_LOW(data.distance[i]) << SHORT_TO_WORD_HIGH(data.distance[i]);
	}
	return os;
}
