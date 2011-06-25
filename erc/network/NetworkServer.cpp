/**
 * NetworkServer listens to port and waits for client connection
 */

#include "NetworkServer.h"
#include "../daemon/Daemon.h"

using namespace std;
using namespace ost;
using namespace erc;

NetworkServer::NetworkServer(tpport_t port, Daemon &daemon)
 : socket(IPV4Host(""),port),daemon(daemon),good(false)
{
	daemon.debug << "NetworkServer: listen on port " << port << "." << endl;
	good = true;
}

NetworkServer::~NetworkServer()
{
	terminate();
	daemon.debug << "NetworkServer: terminated." << endl;
}

bool NetworkServer::isGood() const
{
	return good;
}

void NetworkServer::run()
{
	while(good && socket.isPendingConnection())
	{
		NetworkSession *session = new NetworkSession(socket,daemon,online);
		online.push_back(session);
		session->detach();
	}
}
