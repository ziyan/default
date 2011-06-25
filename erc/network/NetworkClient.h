/**
 * NetworkClient is used to connect to NetworkSession on the server
 */

#ifndef ERC_NETWORKCLIENT_H
#define ERC_NETWORKCLIENT_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cc++/socket.h>
#include "../modules/DataPointer.h"

namespace erc
{
	class NetworkClient : public ost::TCPStream
	{
	public:
		NetworkClient(const ost::IPV4Host &host, const ost::tpport_t port);
		~NetworkClient();
		bool isGood();

		void halt();
		std::istream* getDebugStream();
		std::vector<std::string> getActiveClients();

		DataPointer getModuleData(const std::string&, const unsigned char);
		bool setModuleData(const std::string&, const unsigned char, DataPointer);
		unsigned char getModuleStatus(const std::string&);

		std::map<std::string,unsigned char> getLoadedModules();
		std::map<std::string,unsigned char> getAvailableModules();

		bool loadModule(const std::string&);
		bool unloadModule(const std::string&);

		bool startControl();
		unsigned char getControlStatus();
		bool stopControl();

		std::string getStringConfig(const std::string &key);
		bool setStringConfig(const std::string &key, const std::string &value);
		int getIntConfig(const std::string &key);
		bool setIntConfig(const std::string &key, const int value);
		bool getBoolConfig(const std::string &key);
		bool setBoolConfig(const std::string &key, const bool value);
		double getDoubleConfig(const std::string &key);
		bool setDoubleConfig(const std::string &key, const double value);

	private:
		void command(unsigned char id);
		unsigned char response();
		void sendString(const std::string &word);
		const std::string receiveString();
		bool good;
	};
}

#endif

