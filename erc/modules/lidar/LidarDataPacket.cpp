#include "LidarDataPacket.h"
#include "../../common/define.h"
#include "../../common/crc16.h"
#include <iostream>
#include <cc++/thread.h>

using namespace erc;
using namespace ost;
using namespace std;

static Mutex mutex;

LidarDataPacket::LidarDataPacket()
  : length(0),data(0),addr(0),valid(false)
{
}

LidarDataPacket::LidarDataPacket(const LidarDataPacket& other)
  : length(0),data(0),addr(0),valid(false)
{
	mutex.enterMutex();
	addr = other.addr;
	length = other.length;
	valid = other.valid;
	if(length>0)
	{
		data = new unsigned char[length];
		copy(other.data,other.data+length,data);
	}
	mutex.leaveMutex();
}
LidarDataPacket &LidarDataPacket::operator=(const LidarDataPacket& other)
{
	mutex.enterMutex();
	addr = other.addr;
	length = other.length;
	valid = other.valid;
	delete [] data;
	data=0;
	if(length>0)
	{
		data = new unsigned char[length];
		copy(other.data,other.data+length,data);
	}
	mutex.leaveMutex();
	return *this;
}

LidarDataPacket::~LidarDataPacket()
{
	delete [] data;
}

istream& erc::operator>>(istream &is, LidarDataPacket &p)
{
	//STX
	char stx;
	while(is.get(stx)&&stx!=LidarDataPacket::STX);

	//address
	char addr = 0;
	if(!is.get(addr)) return is;

	//length
	char lenl = 0;
	if(!(is.get(lenl))) return is;
	char lenh = 0;
	if(!(is.get(lenh))) return is;  
	unsigned short int len = WORD_TO_SHORT(lenl,lenh);

	//data
	char *data = new char[len];
	unsigned char *temp = new unsigned char[len+4];
	for(int i=0;i<len;++i)
	{
		if(!is) break;
		is.get(data[i]);
		temp[i+4]=data[i];
	}

	if(!is)
	{
		delete [] data;
		delete [] temp;
		return is;
	}

	//calculate the crc checksum
	char crcl,crch;
	is.get(crcl);
	is.get(crch);
	if(!is)
	{
		delete [] data;
		delete [] temp;
		return is;
	}
	if(crc16(temp,0,len+4)==WORD_TO_SHORT(crcl,crch))
	{
		p.valid = true;
	}
	else
	{
		p.valid = false;
	}

	//put data into the packet
	delete [] p.data;
	delete [] temp;
	p.data = (unsigned char*)data;
	p.length = len;
	p.addr = (unsigned char)addr;

	return is;
}
ostream& erc::operator<<(ostream &os, const LidarDataPacket &p)
{
	//STX
	os << LidarDataPacket::STX;
	//addr
	os << p.addr;
	//length
	unsigned char lenl = SHORT_TO_WORD_LOW(p.length);
	unsigned char lenh = SHORT_TO_WORD_HIGH(p.length);
	os << lenl << lenh;
	//data
	for(int i=0;i<p.length;++i) os << p.data[i];
	//crc
	unsigned char *temp = new unsigned char[p.length+4];
	temp[0]=LidarDataPacket::STX;
	temp[1]=p.addr;
	temp[2]=lenl;
	temp[3]=lenh;
	copy(p.data,p.data+p.length,temp+4);
	short int crc = crc16(temp,0,p.length+4);
	os << SHORT_TO_WORD_LOW(crc) << SHORT_TO_WORD_HIGH(crc);
	delete [] temp;
	return os;
}
