/**
 * Module that runs remotely
 */

 
#ifndef ERC_REMOTEMODULE_H
#define ERC_REMOTEMODULE_H

#include <string>
#include <vector>
#include <libconfig.h++>
#include <cc++/thread.h>

#include "../common/DebugStream.h"
#include "DataPointer.h"
#include "../network/NetworkClient.h"

namespace erc
{
	/**
	 * A base class for remote modules
	 */
	class RemoteModule : public ost::Thread
	{
	public:
		RemoteModule(DebugStream &debug,const std::string host, const unsigned short int port, const std::string name, const unsigned char arg, const unsigned short int sleepiness);
		~RemoteModule();

		bool isGood();
		void close();
		DataPointer getData(const unsigned char arg = 0) const;
		bool setData(DataPointer pointer, const unsigned char arg = 0);

	private:
		NetworkClient in;
		NetworkClient out;

		ost::Mutex inMutex;
		ost::Mutex outMutex;

		void run();
		void final();

		const std::string host;
		const unsigned short int port;
		const std::string name;
		const unsigned char arg;
		const unsigned short int sleepiness;

		DebugStream &debug;
		std::vector<DataPointer> currentData;
		bool good;

	};
}

#endif
