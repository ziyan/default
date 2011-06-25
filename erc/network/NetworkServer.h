/**
 * NetworkServer listens to port and waits for client connection
 */

#ifndef ERC_NETWORKSERVER_H
#define ERC_NETWORKSERVER_H
#include <cc++/thread.h>
#include <cc++/socket.h>
#include <vector>
#include "../common/DebugStream.h"
#include "NetworkSession.h"

namespace erc
{
	class Daemon;
	class NetworkServer : public ost::Thread
	{
	public:
		NetworkServer(ost::tpport_t port, Daemon &daemon);
		~NetworkServer();
		bool isGood() const;
	protected:
		void run();
	private:
		bool good;
		Daemon &daemon;
		ost::TCPSocket socket;
		std::vector<NetworkSession*> online;
	};
}

#endif
