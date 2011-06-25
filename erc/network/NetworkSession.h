/**
 * NetworkSession is created as a seperate thread to talk to the client
 */

#ifndef ERC_NETWORKSESSION_H
#define ERC_NETWORKSESSION_H
#include <vector>
#include <cc++/thread.h>
#include <cc++/socket.h>
#include "../common/DebugStream.h"

namespace erc
{
	class Daemon;
	class NetworkSession : public ost::TCPSession
	{
	public:
		NetworkSession(ost::TCPSocket &socket, Daemon &daemon, std::vector<NetworkSession*> &online);
		~NetworkSession();
		ost::tpport_t getPort() const;
		ost::InetHostAddress getHost() const;
	protected:
		void run();
		void final();
	private:
		void response(unsigned char code);
		void sendString(const std::string &word);
		const std::string receiveString();
		
		Daemon &daemon;
		std::vector<NetworkSession*> &online;
		ost::tpport_t port;
		ost::InetHostAddress host;

	};
}

#endif
