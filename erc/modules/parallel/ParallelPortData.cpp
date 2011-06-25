#include "ParallelPortData.h"
#include "../ModulesDefine.h"

using namespace std;
using namespace erc;

ParallelPortData::ParallelPortData()
{
}

ParallelPortData::~ParallelPortData()
{
}

unsigned char ParallelPortData::getType() const
{
	return DATATYPE_PARALLELPORTDATA;
}

istream &erc::operator>>(istream &is, ParallelPortData &data)
{
	char d,s;
	is.get(d);
	is.get(s);
	if(!is)
	{
		return is;
	}
	for(int i=0;i<8;i++)
	{
		data.data[i] = ((unsigned char)d >> i) & 0x1;
	}
	for(int i=0;i<5;i++)
	{
		data.status[i] = ((unsigned char)s >> i) & 0x1;
	} 
	return is;
}
ostream &erc::operator<<(ostream &os, const ParallelPortData &data)
{
	unsigned char d = 0 ,s = 0;
	for(int i=0;i<8;i++)
	{
		d |= data.data[i] << i;
	}
	for(int i=0;i<5;i++)
	{
		s |= data.status[i] << i;
	}
	os<<d<<s;
	return os;
}
