/**
 * Daemon that manipulates all the modules
 */

#include "Daemon.h"

using namespace std;
using namespace erc;
using namespace ost;
using namespace libconfig;


/** constructor for Daemon */
Daemon::Daemon(DebugStream &debug, const short int port, libconfig::Config &config) :
	good(false),
	server(port,*this),
	control(0),
	debug(debug),
	config(config)
{
	if(!server.isGood())
	{
		debug<<"Daemon: failed to start up network server."<<endl;
		return;
	}
	server.start();
	debug<<"Daemon: initialized."<<endl;
	good = true;
}

/** Destructor for Daemon, also destructs loaded modules */
Daemon::~Daemon()
{
	stopControl();
	while(modules.size()>0)
	{
		unloadModule(modules.begin()->first);
	}
	debug<<"Daemon: destructed."<<endl;
}

/** load a module */
bool Daemon::loadModule(const string& name)
{
	if(getModule(name))
	{
		debug<<"Daemon: module "<<name<<" already loaded."<<endl;
		return false;
	}
	if(!existModule(name))
	{
		debug<<"Daemon: module "<<name<<" not found."<<endl;
		return false;
	}

	Module *module = 0;
	try
	{
		module = new Module(debug,name,(int)config.lookup(string("module.")+name)["type"] & 0xFF,config.lookup(string("module.")+name));
	}
	catch(...)
	{
		if(module!=0)
		{
			delete module;
			return false;
		}
		debug<<"Daemon: module "<<name<<" load failed."<<endl;
		return false;
	}
	//begin start up sequence
	if(module!=0)
	{
		if(!module->isGood())
		{
			debug<<"Daemon: module "<<name<<" load failed."<<endl;
			delete module;
			return false;
		}
		mutex.enterMutex();
		modules.insert(make_pair(name,module));
		mutex.leaveMutex();
		debug<<"Daemon: module "<<name<<" started."<<endl;
		return true;
	}
	return false;
}

bool Daemon::existModule(const string &name)
{
	try
	{
		bool exits = false;
		for(int i=0;i<config.lookup("modules").getLength();i++)
		{
			if((string)config.lookup("modules")[i] == name)
			{
				exits = true;
				break;
			}
		}
		if(!exits) return false;
		if((int)config.lookup(string("module.")+name)["type"]>0
			&& (int)config.lookup(string("module.")+name)["type"]<255)
		{
			return true;
		}
	}
	catch(...)
	{
		return false;
	}
	return true;
}

/** unload a module */
void Daemon::unloadModule(const string &name)
{
	mutex.enterMutex();
	map<string, Module*>::iterator m = modules.find(name);
	if(m!=modules.end())
	{
		if(m->second!=0)
		{
			try
			{
				delete m->second;
			}
			catch(...)
			{
			}
		}
		modules.erase(m);
		debug<<"Daemon: module "<<name<<" unloaded."<<endl;
	}
	mutex.leaveMutex();
}

/** get the pointer of a module */
Module* Daemon::getModule(const string &name) const
{
	map<string, Module*>::const_iterator m = modules.find(name);
	if(m!=modules.end())
		return m->second;
	else
		return 0;
}


/** return a list of available modules */
const map<string,unsigned char> Daemon::getAvailableModules()
{
	map<string,unsigned char> list;
	try
	{
		for(int i=0;i<config.lookup("modules").getLength();i++)
		{
			try
			{
				string name = (string)config.lookup("modules")[i];
				if((int)config.lookup(string("module.")+name)["type"]>0
					&& (int)config.lookup(string("module.")+name)["type"]<255)
				{
					list.insert(make_pair(name,(int)config.lookup(string("module.")+name)["type"] & 0xFF));
				}
			}
			catch(...)
			{
			}
		}
	}
	catch(...)
	{
	}
	return list;
}

/** return a list of loaded modules */
const std::map<string, Module*> Daemon::getLoadedModules() const
{
	return modules;
}

/** return the status of the daemon */
bool Daemon::isGood() const
{
	return good;
}

/** start the control thread */
bool Daemon::startControl()
{
	if(control) return false;
	control = new Control(*this);
	if(control->isGood())
	{
		control->start();
		debug<<"Daemon: control loop started."<<endl;
		return true;
	}
	stopControl();
	debug<<"Daemon: failed to start control loop."<<endl;
	return false;
}

/** get the pointer to the control thread */
Control *Daemon::getControl() const
{
	return control;
}

/** stop the control thread */
void Daemon::stopControl()
{
	if(!control) return;
	control->stop();
	delete control;
	control = 0;
	debug<<"Daemon: control loop stopped."<<endl;
}

