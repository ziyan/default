/** 
 * Wrapper for modules
 */

#include "Module.h"
#include "Modules.h"

using namespace erc;
using namespace std;
using namespace libconfig;

Module::Module(DebugStream &debug,string name, const unsigned char type, Setting &setting) :
	name(name),
	type(type),
	setting(setting),
	local(0),
	remote(0),
	debug(debug)
{
	
	try
	{
		string host, remoteName;
		unsigned short int port, argument, sleep;
		host = (string)setting["host"];
		port = (int)setting["port"];
		remoteName = (string)setting["remote"];
		argument = (int)setting["argument"];
		sleep = (int)setting["sleep"];
		
		debug << "Module: connecting "<<host<<":"<<port<<"."<<endl;
		
		remote = new RemoteModule(debug, host, port, remoteName, argument, sleep);
		remote->start();
		
		debug << "Module: remote module "<<host<<":"<<port<<" "<<remoteName<<" started."<<endl;
		return;
	}
	catch(...)
	{
		//local module
		//switch on type
		switch(type)
		{
			case MODULE_MOTOR_MBLAZE:
				break;
			case MODULE_MOTOR_PHIDGETS:
				local = new PhidgetMC(debug,setting);
				break;
			case MODULE_MOTOR_SERIAL:
				local = new SerialMC(debug,setting);
				break;
			case MODULE_GPS:
				local = new Gps(debug,setting);
				break;
			case MODULE_LIDAR:
				local = new Lidar(debug,setting);
				break;
			case MODULE_CAMERA:
				local = new Camera(debug,setting);
				break;
			case MODULE_COMPASS:
				local = new Compass(debug,setting);
				break;
			case MODULE_JOYSTICK:
				local = new JoyStick(debug,setting);
				break;
			case MODULE_JAUS:
				local = new Jaus(debug,setting);
				break;
			case MODULE_PARALLELPORT:
				local = new ParallelPort(debug,setting);
				break;
			default:
				return;
		}
		if(local)
		{
			if(!(local->isGood())) return;
			if(!(local->prepare())) return;
			local->start();
			if(!(local->isReady())) return;
		}
		return;
	}
}

Module::~Module()
{
	if(remote)
	{
		remote->close();
	}
	if(local)
	{
		local->close();
	}
	delete remote;
	delete local;
}

bool Module::isGood() const
{
	if(remote)
	{
		return remote->isGood();
	}
	if(local)
	{
		return local->isGood();
	}
	return false;
}

DataPointer Module::getData(const unsigned char arg) const
{
	if(remote)
	{
		return remote->getData(arg);
	}
	if(local)
	{
		return local->getData(arg);
	}
	return DataPointer(0);
}

bool Module::setData(DataPointer pointer, const unsigned char arg)
{
	if(remote)
	{
		return remote->setData(pointer,arg);
	}
	if(local)
	{
		return local->setData(pointer,arg);
	}
	return false;
}
