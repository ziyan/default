/**
 * Daemon that manipulates all the modules
 */

#ifndef ERC_DAEMON_H
#define ERC_DAEMON_H

#include "../common/DebugStream.h"
#include "../modules/Module.h"
#include "../network/NetworkServer.h"
#include "../control/Control.h"

#include <map>
#include <iostream>
#include <cc++/thread.h>
#include <string>
#include <libconfig.h++>

namespace erc
{
	class Daemon
	{
		friend class NetworkSession;
		friend class Control;
	public:
		/**
		 * Constructor
		 */
		Daemon(DebugStream &debug, const short int port, libconfig::Config &config);
		/**
		 * Destructor, unload all modules and sql connection and network server
		 */
		~Daemon();

		/**
		 * Explicitly load a module into our module cache
		 */
		bool loadModule(const std::string&);
		
		
		bool existModule(const std::string&);
		/**
		 * Unload a module from cache
		 */
		void unloadModule(const std::string&);
		/**
		 * Returns a pointer to a specific module
		 */
		Module* getModule(const std::string&) const;
		/**
          * Returns configuration information
          */
          
        template <class T> const T getConfig(const std::string &key)
        {
        	T value;
        	try
			{
				value = (T)config.lookup(key);
			}
			catch(...)
			{
			}
			return value;
        }
        template <class T> void setConfig(const std::string &key, const T value)
        {
        	try
        	{
        		config.lookup(key) = value;
        		debug << "Daemon: configuration "<<key<<" = "<<value << std::endl;
        	}
        	catch(...)
        	{
        	}
        }
       	

		/**
		 * Available module list
		 */
		const std::map<std::string,unsigned char> getAvailableModules();
		/**
		 * Loaded module
		 */
		const std::map<std::string, Module*> getLoadedModules() const;

		/**
		 * This function blocks, when it exits, the daemon is terminated
		 */
		void start();
		/**
		 * Error flag
		 */
		bool isGood() const;

		/**
		 * Start the control loop
		 */
		bool startControl();
		/**
		 * Stop the control loop
		 */
		void stopControl();
		/**
		 * Get the pointer to the control loop
		 */
		Control *getControl() const;

		/** Debug output stream */
		DebugStream &debug;

		libconfig::Config &config;
		
	private:
		bool good;
		ost::Mutex mutex;
		std::map<std::string, Module*> modules;
		NetworkServer server;
		Control *control;
	};
}

#endif
