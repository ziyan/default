#include "MotorData.h"
#include "../ModulesDefine.h"

using namespace std;
using namespace erc;

MotorData::MotorData()
 : value(0),num(0)
{
}

MotorData::~MotorData()
{
	delete [] value;
}

unsigned char MotorData::getType() const
{
	return DATATYPE_MOTORDATA;
}

istream &erc::operator>>(istream &is, MotorData &data)
{
	char c, len;
	is.get(len);
	if(!is) return is;
	unsigned char *value = new unsigned char[(int)(unsigned char)len];
	for(int i=0;i<(int)(unsigned char)len;i++)
	{
		is.get(c);
		value[i]=(unsigned char)c;
	}
	if(!is)
	{
		delete [] value;
		return is;
	}
	delete [] data.value;
	data.num = (unsigned char)len;
	data.value = value;
	return is;
}
ostream &erc::operator<<(ostream &os, const MotorData &data)
{
	os<<data.num;
	for(int i=0;i<(int)data.num;i++) os << data.value[i];
	return os;
}
