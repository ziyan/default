/**
 * Module that runs remotely
 */

#include "RemoteModule.h"

using namespace std;
using namespace erc;
using namespace ost;

RemoteModule::RemoteModule(DebugStream &debug,const std::string host, const unsigned short int port, const string name, const unsigned char arg, const unsigned short int sleepiness) :
	in(host.c_str(),int(port)),
	out(host.c_str(),int(port)),
	host(host),
	port(port),
	name(name),
	arg(arg),
	currentData((int)arg),
	sleepiness(sleepiness),
	debug(debug),
	good(false)
{
	for(int i=0;i<arg;i++)
	{
		currentData[i] = DataPointer(0);
	}
	good = true;
}

RemoteModule::~RemoteModule()
{
}

bool RemoteModule::isGood()
{
	return good && in.isGood() && out.isGood();
}

void RemoteModule::close()
{
	good = false;
	inMutex.enterMutex();
	inMutex.leaveMutex();
}


DataPointer RemoteModule::getData(const unsigned char arg) const
{
	if((int)arg<currentData.size())
		return currentData[(int)arg];
	else
		return DataPointer(0);
}

bool RemoteModule::setData(DataPointer pointer, const unsigned char arg)
{
	outMutex.enterMutex();
	out.setModuleData(name,arg,pointer);
	outMutex.leaveMutex();
	return true;
}

void RemoteModule::run()
{
	while(isGood())
	{
		inMutex.enterMutex();
		for(int i=0;i<currentData.size();i++)
		{
			currentData[i] = in.getModuleData(name,i);
		}
		inMutex.leaveMutex();
		if(!(in.isGood())) break;
		if(sleepiness>0)
			Thread::sleep(sleepiness);
		else
			Thread::sleep(500);
	}
}

void RemoteModule::final()
{
}



