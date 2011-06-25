#include "Gps.h"
#include "GpsData.h"
#include "UTMConversion.h" //Conversion
#include <cstdio>

using namespace std;
using namespace erc;

Gps::Gps(DebugStream& debug,libconfig::Setting &setting)
  : LocalModule(debug,setting),gps(0),good(false),currentData(new GpsData)
{
	
	string host;
	unsigned short int port;
	host = (string)setting["host"];
	port = (int)setting["port"];
	debug<<"Gps: connecting to "<<host<<":"<<port<<endl;
	char portp[6];
	sprintf(portp,"%i",port);
	gps = gps_open(host.c_str(), portp);
	good = (gps);
}

Gps::~Gps()
{
}

bool Gps::isReady()
{
	for(int i=0;i<50;i++)
	{
		debug<<"Gps: trying to get fixed data. ("<<(100.0*i/1000.0)<<"s)"<<endl;
		if(!isGood()) break;
		if(gps->status != STATUS_NO_FIX) break;
		Thread::sleep(100);
	}
	good = isGood() && (gps->status != STATUS_NO_FIX);
	return isGood() && (gps->status != STATUS_NO_FIX);
}

bool Gps::isGood() const
{
	return good && (gps);
}

void Gps::run()
{
	while(isGood())
	{
		mutex.enterMutex();
		if(gps_query(gps,"o")<0) break;
		if(gps->status != STATUS_NO_FIX)
		{
			GpsData &data = (GpsData&)(*currentData);
			if(gps->fix.latitude==gps->fix.latitude) data.latitude = gps->fix.latitude;
			if(gps->fix.longitude==gps->fix.longitude) data.longitude = gps->fix.longitude;
			if(gps->fix.speed==gps->fix.speed) data.speed = gps->fix.speed;
			if(gps->fix.altitude==gps->fix.altitude) data.altitude = gps->fix.altitude;
			if(gps->fix.climb==gps->fix.climb) data.climb = gps->fix.climb;
			LL2UTM(data,data);
			//debug<<"Gps: got fixed data."<<endl;
		}
		mutex.leaveMutex();
		Thread::sleep(10);
	}
	good = false;
}

DataPointer Gps::getData(const unsigned char arg) const
{
	return currentData;
}

void Gps::close()
{
	good = false;
	Thread::sleep(100);
	mutex.enterMutex();
	if(gps) gps_close(gps);
	mutex.leaveMutex();
}


