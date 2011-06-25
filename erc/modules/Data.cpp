/**
 * Base Class for Data
 */

#include "Data.h"
#include "Modules.h"

using namespace std;
using namespace erc;

istream &erc::operator>>(istream &in, Data &data)
{
	switch(data.getType())
	{
		case DATATYPE_GPSDATA:
			in >> (GpsData&)data;
			break;
		case DATATYPE_MOTORDATA:
			in >> (MotorData&)data;
			break;
		case DATATYPE_COMPASSDATA:
			in >> (CompassData&)data;
			break;
		case DATATYPE_LIDARDATA:
			in >> (LidarData&)data;
			break;
		case DATATYPE_CAMERADATA:
			in >> (CameraData&)data;
			break;
		case DATATYPE_CAMERAFRAME:
			in >> (CameraFrame&)data;
		case DATATYPE_JOYSTICKDATA:
			in >> (JoyStickData&)data;
			break;
		case DATATYPE_JAUSDATA:
			in >> (JausData&)data;
			break;
		case DATATYPE_PARALLELPORTDATA:
			in >> (ParallelPortData&)data;
			break;
	}
	return in;
}
ostream &erc::operator<<(ostream &out, const Data &data)
{
	switch(data.getType())
	{
		case DATATYPE_GPSDATA:
			out << (GpsData&)data;
			break;
		case DATATYPE_MOTORDATA:
			out << (MotorData&)data;
			break;
		case DATATYPE_COMPASSDATA:
			out << (CompassData&)data;
			break;
		case DATATYPE_LIDARDATA:
			out << (LidarData&)data;
			break;
		case DATATYPE_CAMERADATA:
			out << (CameraData&)data;
			break;
		case DATATYPE_CAMERAFRAME:
			out << (CameraFrame&)data;
			break;
		case DATATYPE_JOYSTICKDATA:
			out << (JoyStickData&)data;
			break;
		case DATATYPE_JAUSDATA:
			out << (JausData&)data;
			break;
		case DATATYPE_PARALLELPORTDATA:
			out << (ParallelPortData&)data;
			break;
	}
	return out;
}

Data::Data()
 : refCounter(0)
{
}

Data::~Data()
{
}

