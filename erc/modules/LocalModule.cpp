/**
 * Module that runs locally
 */

#include "LocalModule.h"

using namespace std;
using namespace ost;
using namespace erc;

LocalModule::LocalModule(DebugStream& debug, libconfig::Setting &setting) :
	debug(debug),
	setting(setting)
{
}

LocalModule::~LocalModule()
{
}

bool LocalModule::prepare()
{
	return true;
}

bool LocalModule::isGood() const
{
	return true;
}

bool LocalModule::isReady()
{
	return true;
}

void LocalModule::close()
{
}

DataPointer LocalModule::getData(const unsigned char arg) const
{
	return DataPointer(0);
}

bool LocalModule::setData(DataPointer pointer, const unsigned char arg)
{
	return false;
}

void LocalModule::run()
{
	while(isGood())
		Thread::sleep(1000);
}

void LocalModule::final()
{
}

