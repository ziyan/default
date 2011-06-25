/**
 * Smart Pointer for Data
 */

#include "DataPointer.h"
#include "Modules.h"

using namespace std;
using namespace erc;


istream &erc::operator>>(istream &in, DataPointer &pointer)
{
	char c;
	in.get(c);
	unsigned char type = (unsigned char)c;
	if(type == 0) return in;
	if(pointer.isNull()||pointer->getType()!=type)
	{
		switch(type)
		{
			case DATATYPE_GPSDATA:
				pointer = DataPointer(new GpsData);
				break;
			case DATATYPE_MOTORDATA:
				pointer = DataPointer(new MotorData);
				break;
			case DATATYPE_COMPASSDATA:
				pointer = DataPointer(new CompassData);
				break;
			case DATATYPE_LIDARDATA:
				pointer = DataPointer(new LidarData);
				break;
			case DATATYPE_CAMERADATA:
				pointer = DataPointer(new CameraData);
				break;
			case DATATYPE_CAMERAFRAME:
				pointer = DataPointer(new CameraFrame);
				break;
			case DATATYPE_JOYSTICKDATA:
				pointer = DataPointer(new JoyStickData);
				break;
			case DATATYPE_JAUSDATA:
				pointer = DataPointer(new JausData);
				break;
			case DATATYPE_PARALLELPORTDATA:
				pointer = DataPointer(new ParallelPortData);
				break;
			default:
				pointer = DataPointer(0);
				return in;
		}
		
	}
	in >> (*pointer);
	return in;
}

ostream &erc::operator<<(ostream &out, const DataPointer &pointer)
{
	if(!pointer.isNull())
	{
		out << pointer->getType();
		out << (*pointer);
	}
	else
	{
		out << (unsigned char)0;
	}
	return out;
}

DataPointer::DataPointer(Data *data)
 : data(data)
{
	if(data)
	{
		data->mutex.enterMutex();
		data->refCounter++;
		data->mutex.leaveMutex();
	}
}

DataPointer::DataPointer(const DataPointer &other)
 : data(data)
{
	data = other.data;
	if(data)
	{
		data->mutex.enterMutex();
		data->refCounter++;
		data->mutex.leaveMutex();
	}
}

DataPointer::~DataPointer()
{
	if(data)
	{
		data->mutex.enterMutex();
		data->refCounter--;
		data->mutex.leaveMutex();
		if(data->refCounter<=0)
		{
			delete data;
		}
	}
}

bool DataPointer::isNull() const
{
    return data == NULL;
}

DataPointer &DataPointer::operator=(const DataPointer &other)
{
	if(data)
	{
		data->mutex.enterMutex();
		data->refCounter--;
		data->mutex.leaveMutex();
		if(data->refCounter<=0)
		{
			delete data;
		}
	}
	data = other.data;
	if(data)
	{
		data->mutex.enterMutex();
		data->refCounter++;
		data->mutex.leaveMutex();
	}
}

bool DataPointer::operator==(const DataPointer &other) const
{
	return data == other.data;
}

bool DataPointer::operator!=(const DataPointer &other) const
{
	return data != other.data;
}

Data &DataPointer::operator*() const
{
	return *data;
}

Data *DataPointer::operator->() const
{
	return data;
}
