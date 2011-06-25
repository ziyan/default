#include "GpsData.h"
#include "UTMConversion.h"
#include "../../common/define.h"
#include "../ModulesDefine.h"
#include <cmath>

using namespace std;
using namespace erc;

unsigned char GpsData::getType() const
{
	return DATATYPE_GPSDATA;
}

void GpsData::updateUTM()
{
	LL2UTM(*this, *this);
}

double GpsData::operator-(const GpsData &data) const
{
	return hypot((data.x-x), (data.y-y));
}

double GpsData::operator/(const GpsData &data) const
{
	double dy = data.y - y;
	double dx = data.x - x;
	dy = -dy;

	if(dy==0)
	{
		if(dx>0)
			return 90.0;
		else if(dx<0)
			return -90.0;
		else
			return 0.0;
	}
	
	if(dx==0)
	{
		if(dy>0)
			return 0.0;
		else if(dy<0)
			return 180.0;
		else
			return 0.0;
	}

	double angle = RAD_TO_DEG(atan(fabs(dy)/fabs(dx)));
	if(dy>0)
	{
		if(dx>0)
			return 90.0-angle;
		else
			return -(90.0-angle);
	}
	else
	{
		if(dx>0)
			return 90.0+angle;
		else
			return -90.0-angle;
	}
}

istream &erc::operator>>(istream &is, GpsData &data)
{
	double lat,lon,spe,alt,cli,x,y;
	char zone[4];
	is >> lat >> lon >> spe >> alt >> cli >> x >> y >> zone;
	if(!is) return is;
	data.latitude = lat;
	data.longitude = lon;
	data.speed = spe;
	data.altitude = alt;
	data.climb = cli;
	data.x = x;
	data.y = y;
	copy(zone,zone+4,data.zone);
	return is;
}
ostream &erc::operator<<(ostream &os, const GpsData &data)
{
	os << data.latitude << endl;
	os << data.longitude << endl;
	os << data.speed << endl;
	os << data.altitude << endl;
	os << data.climb << endl;
	os << data.x << endl;
	os << data.y << endl;
	os << data.zone << endl;
	return os;
}
