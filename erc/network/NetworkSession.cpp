/**
 * NetworkSession is created as a seperate thread to talk to the client
 */

#include "NetworkSession.h"
#include "../daemon/Daemon.h"
#include "../common/define.h"
#include "NetworkDataPacket.h"
#include "NetworkDefine.h"
#include <cc++/thread.h>

using namespace erc;
using namespace std;
using namespace ost;

static Mutex mutex;

NetworkSession::NetworkSession(TCPSocket &socket,Daemon &daemon,vector<NetworkSession*> &online)
 : TCPSession(socket),daemon(daemon),online(online),host(getPeer(&port))
{
	daemon.debug<<"Network: client "<<host<<":"<<port<<" connected. "<<(online.size()+1)<<" active clients."<<endl;
}

NetworkSession::~NetworkSession()
{
	daemon.debug.dereg(tcp());
	for(vector<NetworkSession*>::iterator i=online.begin();i!=online.end();++i)
	{
		if(this==*i)
		{
			online.erase(i);
			break;
		}
	}
	terminate();
	daemon.debug<<"Network: client "<<host<<":"<<port<<" disconnected. "<<online.size()<<" active clients."<<endl;
}

tpport_t NetworkSession::getPort() const
{
	return port;
}
InetHostAddress NetworkSession::getHost() const
{
	return host;
}
void NetworkSession::run()
{
	//check for version
	char version;
	(*tcp()).get(version);
	(*tcp()) << (unsigned char)VERSION;
	(*tcp()).flush();
	if(!(*tcp()) || (unsigned char)version!=(unsigned char)VERSION)
	{
		daemon.debug << "Network: remote client version incorrect. ("<<(int)version<<")"<<endl;
		return;
	}
	
	while(*tcp())
	{
		NetworkDataPacket p;
		*tcp()>>p;

		switch(p.id)
		{
			case NETWORK_CMD_DISCONNECT:
				return;
			case NETWORK_CMD_SYSTEM_HALT:
				daemon.good = false;
				return;
			case NETWORK_CMD_CONTROL_START:
			{
				if(daemon.startControl())
					response(NETWORK_CODE_SUCCESS);
				else
					response(NETWORK_CODE_FAILURE);
				break;
			}
			case NETWORK_CMD_CONTROL_STATUS:
			{
				response(NETWORK_CODE_SUCCESS_WITH_DATA);
				if(daemon.getControl() && daemon.getControl()->isGood())
					*tcp()<<(unsigned char)NETWORK_STATUS_RUNNING;
				else if(daemon.getControl())
					*tcp()<<(unsigned char)NETWORK_STATUS_ERROR;
				else
					*tcp()<<(unsigned char)NETWORK_STATUS_NOT_STARTED;
				(*tcp()).flush();
				break;
			}
			case NETWORK_CMD_CONTROL_STOP:
			{
				daemon.stopControl();
				response(NETWORK_CODE_SUCCESS);
				break;
			}
			case NETWORK_CMD_MODULE_AVAILABLE:
			{
				response(NETWORK_CODE_SUCCESS_WITH_DATA);
				const std::map<string,unsigned char> m = daemon.getAvailableModules();
				map<string,unsigned char>::const_iterator i;
				*tcp() << SHORT_TO_WORD_LOW(m.size()) << SHORT_TO_WORD_HIGH(m.size());
				for(i=m.begin();i!=m.end();++i)
				{
					sendString(i->first);
					*tcp() << (i->second);
				}
				(*tcp()).flush();
				break;
			}
			case NETWORK_CMD_MODULE_LOADED:
			{
				response(NETWORK_CODE_SUCCESS_WITH_DATA);
				const std::map<string, Module*> m = daemon.getLoadedModules();
				map<string,Module*>::const_iterator i;
				*tcp() << SHORT_TO_WORD_LOW(m.size()) << SHORT_TO_WORD_HIGH(m.size());
				for(i=m.begin();i!=m.end();++i)
				{
					sendString(i->first);
					*tcp() << (i->second->type);
				}
				(*tcp()).flush();
				break;
			}
			case NETWORK_CMD_MODULE_LOAD:
			{
				const string& name = receiveString();
				if(!(*tcp())) break;

				Module *m = daemon.getModule(name);
				if(m)
					response(NETWORK_CODE_FAILURE);
				else
					if(daemon.loadModule(name))
						response(NETWORK_CODE_SUCCESS);
					else
						response(NETWORK_CODE_FAILURE);
				break;
			}
			case NETWORK_CMD_MODULE_UNLOAD:
			{
				const string& name = receiveString();
				if(!(*tcp())) break;

				Module *m = daemon.getModule(name);
				if(!m)
					response(NETWORK_CODE_FAILURE);
				else
				{
					daemon.unloadModule(name);
					response(NETWORK_CODE_SUCCESS);
				}
				break;
			}
			case NETWORK_CMD_MODULE_GET:
			{
				const string& name = receiveString();
				char arg;
				(*tcp()).get(arg);
				if(!(*tcp())) break;

				Module *m = daemon.getModule(name);
				if(!m)
					response(NETWORK_CODE_FAILURE);
				else
				{
					response(NETWORK_CODE_SUCCESS_WITH_DATA);
					*tcp() << (m->getData(arg));
					(*tcp()).flush();
				}
				break;
			}
			case NETWORK_CMD_MODULE_SET:
			{
				const string& name = receiveString();
				char arg;
				(*tcp()).get(arg);
				if(!(*tcp())) break;
				
				Module *m = daemon.getModule(name);
				if(!m)
					response(NETWORK_CODE_FAILURE);
				else
				{
					response(NETWORK_CODE_SUCCESS_WITH_DATA);
					DataPointer ptr;
					*tcp() >> ptr;
					if(!(*tcp())) break;
					m->setData(ptr,arg);
				}
				break;
			}
			case NETWORK_CMD_MODULE_STATUS:
			{
				const string& name = receiveString();
				if(!(*tcp())) break;

				Module *m = daemon.getModule(name);
				response(NETWORK_CODE_SUCCESS_WITH_DATA);

				if(!m)
					*tcp()<<(unsigned char)NETWORK_STATUS_NOT_STARTED;
				else if(m->isGood())
					*tcp()<<(unsigned char)NETWORK_STATUS_RUNNING;
				else
					*tcp()<<(unsigned char)NETWORK_STATUS_ERROR;
				(*tcp()).flush();
				break;
			}
			case NETWORK_CMD_CONFIG_GET_STRING:
			{
				const string& key = receiveString();
				if(!(*tcp())) break;
				string value = daemon.getConfig<string>(key);
				if(value.size()<=0)
					response(NETWORK_CODE_FAILURE);
				else
				{
					response(NETWORK_CODE_SUCCESS_WITH_DATA);
					sendString(value);
					(*tcp()).flush();
				}
				break;
			}
			case NETWORK_CMD_CONFIG_SET_STRING:
			{
				const string& key = receiveString();
				const string& value = receiveString();
				if(!(*tcp())) break;

				if(key.size()<=0)
				{
					response(NETWORK_CODE_FAILURE);
					break;
				}
				daemon.setConfig<string>(key,value);
				response(NETWORK_CODE_SUCCESS);
				break;
			}
			case NETWORK_CMD_CONFIG_GET_BOOL:
			{
				const string& key = receiveString();
				if(!(*tcp())) break;
				response(NETWORK_CODE_SUCCESS_WITH_DATA);
				*tcp() << (daemon.getConfig<bool>(key)?'T':'F');
				(*tcp()).flush();
				break;
			}
			case NETWORK_CMD_CONFIG_SET_BOOL:
			{
				const string& key = receiveString();
				char c;
				(*tcp()).get(c);
				if(!(*tcp())) break;
				daemon.setConfig<bool>(key,c=='T'?true:false);
				response(NETWORK_CODE_SUCCESS);
				break;
			}
			case NETWORK_CMD_CONFIG_GET_INT:
			{
				const string& key = receiveString();
				if(!(*tcp())) break;
				response(NETWORK_CODE_SUCCESS_WITH_DATA);
				ostringstream os;
				os<<daemon.getConfig<int>(key);
				sendString(os.str());
				(*tcp()).flush();
				break;
			}
			case NETWORK_CMD_CONFIG_SET_INT:
			{
				const string& key = receiveString();
				const string& value = receiveString();
				if(!(*tcp())) break;
				daemon.setConfig<int>(key,atoi(value.c_str()));
				response(NETWORK_CODE_SUCCESS);
				break;
			}
			case NETWORK_CMD_CONFIG_GET_DOUBLE:
			{
				const string& key = receiveString();
				if(!(*tcp())) break;
				response(NETWORK_CODE_SUCCESS_WITH_DATA);
				ostringstream os;
				os<<daemon.getConfig<double>(key);
				sendString(os.str());
				(*tcp()).flush();
				break;
			}
			case NETWORK_CMD_CONFIG_SET_DOUBLE:
			{
				const string& key = receiveString();
				const string& value = receiveString();
				if(!(*tcp())) break;
				stringstream ss;
				ss << value;
				double d;
				ss >> d;
				if(!ss)
				{
					response(NETWORK_CODE_FAILURE);
					break;
				}
				daemon.setConfig<double>(key,d);
				response(NETWORK_CODE_SUCCESS);
				break;
			}
			case NETWORK_CMD_DEBUG:
			{
				response(NETWORK_CODE_SUCCESS_WITH_DATA);
				daemon.debug.reg(tcp());
				break;
			}
			case NETWORK_CMD_ONLINE_CLIENT:
			{
				response(NETWORK_CODE_SUCCESS_WITH_DATA);
				*tcp() << SHORT_TO_WORD_LOW(online.size()) << SHORT_TO_WORD_HIGH(online.size());

				for(vector<NetworkSession*>::iterator i=online.begin();i!=online.end();++i)
				{
					unsigned long int ip = ((*i)->getHost()).getAddress().s_addr;
					*tcp() << (unsigned char)(ip >> 0 & 0xff);
					*tcp() << (unsigned char)(ip >> 8 & 0xff);
					*tcp() << (unsigned char)(ip >> 16 & 0xff);
					*tcp() << (unsigned char)(ip >> 24 & 0xff);
					*tcp() << SHORT_TO_WORD_LOW((*i)->getPort()) << SHORT_TO_WORD_HIGH((*i)->getPort());
				}
				
				(*tcp()).flush();
			}
			default:
				break;
		}
	}
	daemon.debug.dereg(tcp());
}

void NetworkSession::final()
{
}

void NetworkSession::response(unsigned char code)
{
	if(*tcp())
	{
		NetworkDataPacket re;
		re.id = code;
		*tcp()<<re;
		(*tcp()).flush();
	}
}

void NetworkSession::sendString(const string &word)
{
	*tcp()<<SHORT_TO_WORD_LOW(word.size())<<SHORT_TO_WORD_HIGH(word.size());
	for(int i=0;i<word.size();i++)
	{
		
		*tcp()<<word[i];
	}
}
const string NetworkSession::receiveString()
{
	ostringstream os;
	if(*tcp())
	{
		char l,h;
		(*tcp()).get(l);
		(*tcp()).get(h);
		unsigned short int len = WORD_TO_SHORT(l,h);
		for(int i=0;i<len;i++)
		{
			(*tcp()).get(l);
			os << l;
		}
	}
	return os.str();
}


