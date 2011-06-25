/**
 * NetworkClient is used to connect to NetworkSession on the server
 */

#include "NetworkClient.h"
#include "../common/define.h"
#include "NetworkDataPacket.h"
#include "NetworkDefine.h"
#include <sstream>

using namespace ost;
using namespace std;
using namespace erc;

NetworkClient::NetworkClient(const IPV4Host &host, const tpport_t port)
 : TCPStream(host,port),good(false)
{
	//check for version
	char version;
	(*tcp()) << (unsigned char)VERSION;
	(*tcp()).flush();
	(*tcp()).get(version);
	if(!(*tcp()) || (unsigned char)version!=(unsigned char)VERSION)
	{
		command(NETWORK_CMD_DISCONNECT);
		TCPStream::disconnect();
		return;
	}
	good = true;
}

NetworkClient::~NetworkClient()
{
	command(NETWORK_CMD_DISCONNECT);
	TCPStream::disconnect();
}

bool NetworkClient::isGood()
{
	return good&&(tcp())&&(*tcp());
}


void NetworkClient::halt()
{
	command(NETWORK_CMD_SYSTEM_HALT);
}

istream* NetworkClient::getDebugStream()
{
	if(!isGood()) return 0;
	command(NETWORK_CMD_DEBUG);
	if(response()!=NETWORK_CODE_SUCCESS_WITH_DATA) return 0;
	return tcp();
}

vector<string> NetworkClient::getActiveClients()
{
	vector<string> v;
	if(!isGood()) return v;
	command(NETWORK_CMD_ONLINE_CLIENT);
	if(response()!=NETWORK_CODE_SUCCESS_WITH_DATA) return v;
	char l,h;
	(*tcp()).get(l);
	(*tcp()).get(h);
	short int num = WORD_TO_SHORT(l,h);
	for(int i=0;i<num;i++)
	{
		ostringstream os;
		(*tcp()).get(l);
		os << ((int)(unsigned char)l) << ".";
		(*tcp()).get(l);
		os << ((int)(unsigned char)l) << ".";
		(*tcp()).get(l);
		os << ((int)(unsigned char)l) << ".";
		(*tcp()).get(l);
		os << ((int)(unsigned char)l) << ":";

		(*tcp()).get(l);
		(*tcp()).get(h);
		if(!(*tcp())) break;
		os << WORD_TO_SHORT(l,h);
		v.push_back(os.str());
	}
	return v;
}


DataPointer NetworkClient::getModuleData(const string &name, const unsigned char arg)
{
	DataPointer ptr;
	if(!isGood()) return ptr;
	command(NETWORK_CMD_MODULE_GET);
	sendString(name);
	*tcp() << arg;
	(*tcp()).flush();
	if(response()!=NETWORK_CODE_SUCCESS_WITH_DATA) return ptr;
	*tcp() >> ptr;
	if(*tcp()) return ptr;
	return DataPointer(0);
}

bool NetworkClient::setModuleData(const string &name, const unsigned char arg, DataPointer pointer)
{
	if(!isGood()) return false;
	command(NETWORK_CMD_MODULE_SET);
	sendString(name);
	*tcp() << arg;
	(*tcp()).flush();
	if(response()!=NETWORK_CODE_SUCCESS_WITH_DATA) return false;
	*tcp() << pointer;
	(*tcp()).flush();
	return true;
}

unsigned char NetworkClient::getModuleStatus(const string &name)
{
	if(!isGood()) return NETWORK_STATUS_UNKNOWN;
	command(NETWORK_CMD_MODULE_STATUS);
	sendString(name);
	(*tcp()).flush();
	if(response()!=NETWORK_CODE_SUCCESS_WITH_DATA) return NETWORK_STATUS_UNKNOWN;

	char s;
	(*tcp()).get(s);
	return (unsigned char)s;
}

map<string,unsigned char> NetworkClient::getLoadedModules()
{
	map<string,unsigned char> list;
	if(!isGood()) return list;
	command(NETWORK_CMD_MODULE_LOADED);
	if(response()!=NETWORK_CODE_SUCCESS_WITH_DATA) return list;

	char l,h;
	(*tcp()).get(l);
	(*tcp()).get(h);
	short int count = WORD_TO_SHORT(l,h);
	for(short int i=0;i<count;i++)
	{
		string name = receiveString();
		(*tcp()).get(l);
		unsigned char type = (unsigned char)l;
		list.insert(make_pair(name,type));
	}
	if(!isGood()) list.clear();
	return list;
}
map<string,unsigned char> NetworkClient::getAvailableModules()
{
	map<string,unsigned char> list;
	if(!isGood()) return list;
	command(NETWORK_CMD_MODULE_AVAILABLE);
	if(response()!=NETWORK_CODE_SUCCESS_WITH_DATA) return list;

	char l,h;
	(*tcp()).get(l);
	(*tcp()).get(h);
	short int count = WORD_TO_SHORT(l,h);
	for(short int i=0;i<count;i++)
	{
		string name = receiveString();
		(*tcp()).get(l);
		unsigned char type = (unsigned char)l;
		list.insert(make_pair(name,type));
	}
	if(!isGood()) list.clear();
	return list;
}
bool NetworkClient::startControl()
{
	if(!isGood()) return false;
	command(NETWORK_CMD_CONTROL_START);
	if(response()!=NETWORK_CODE_SUCCESS_WITH_DATA) return true;
	return true;
}
unsigned char NetworkClient::getControlStatus()
{
	if(!isGood()) return NETWORK_STATUS_UNKNOWN;
	command(NETWORK_CMD_CONTROL_STATUS);
	if(response()!=NETWORK_CODE_SUCCESS_WITH_DATA) return NETWORK_STATUS_UNKNOWN;

	char s;
	(*tcp()).get(s);
	return (unsigned char)s;
}
bool NetworkClient::stopControl()
{
	if(!isGood()) return false;
	command(NETWORK_CMD_CONTROL_STOP);
	if(response()!=NETWORK_CODE_SUCCESS_WITH_DATA) return true;
	return true;
}
bool NetworkClient::loadModule(const string &name)
{
	if(!isGood()) return false;
	command(NETWORK_CMD_MODULE_LOAD);
	sendString(name);
	(*tcp()).flush();
	if(response()!=NETWORK_CODE_SUCCESS) return false;
	return true;
}
bool NetworkClient::unloadModule(const string &name)
{
	if(!isGood()) return false;
	command(NETWORK_CMD_MODULE_UNLOAD);
	sendString(name);
	(*tcp()).flush();
	if(response()!=NETWORK_CODE_SUCCESS) return false;
	return true;
}

std::string NetworkClient::getStringConfig(const string &key)
{
	if(!isGood()) return "";
	command(NETWORK_CMD_CONFIG_GET_STRING);
	sendString(key);
	(*tcp()).flush();
	if(response()!=NETWORK_CODE_SUCCESS_WITH_DATA) return "";
	return receiveString();
}

bool NetworkClient::setStringConfig(const string &key, const string &value)
{
	if(!isGood()) return false;
	command(NETWORK_CMD_CONFIG_SET_STRING);
	sendString(key);
	sendString(value);
	(*tcp()).flush();
	if(response()!=NETWORK_CODE_SUCCESS) return false;
	return true;
}

bool NetworkClient::getBoolConfig(const string &key)
{
	if(!isGood()) return false;
	command(NETWORK_CMD_CONFIG_GET_BOOL);
	sendString(key);
	(*tcp()).flush();
	if(response()!=NETWORK_CODE_SUCCESS_WITH_DATA) return false;
	char c;
	(*tcp()).get(c);
	return c=='T';
}

bool NetworkClient::setBoolConfig(const string &key, const bool value)
{
	if(!isGood()) return false;
	command(NETWORK_CMD_CONFIG_SET_BOOL);
	sendString(key);
	(*tcp()) << (value?'T':'F');
	(*tcp()).flush();
	if(response()!=NETWORK_CODE_SUCCESS) return false;
	return true;
}

int NetworkClient::getIntConfig(const string &key)
{
	if(!isGood()) return -1;
	command(NETWORK_CMD_CONFIG_GET_INT);
	sendString(key);
	(*tcp()).flush();
	if(response()!=NETWORK_CODE_SUCCESS_WITH_DATA) return -1;
	return atoi(receiveString().c_str());
}

bool NetworkClient::setIntConfig(const string &key, const int value)
{
	if(!isGood()) return false;
	command(NETWORK_CMD_CONFIG_SET_INT);
	sendString(key);
	ostringstream os;
	os << value;
	sendString(os.str());
	(*tcp()).flush();
	if(response()!=NETWORK_CODE_SUCCESS) return false;
	return true;
}

double NetworkClient::getDoubleConfig(const string &key)
{
	if(!isGood()) return -1;
	command(NETWORK_CMD_CONFIG_GET_DOUBLE);
	sendString(key);
	(*tcp()).flush();
	if(response()!=NETWORK_CODE_SUCCESS_WITH_DATA) return -1;
	stringstream ss;
	ss << receiveString();
	double d;
	ss >> d;
	if(ss) return d;
	return -1;
}

bool NetworkClient::setDoubleConfig(const string &key, const double value)
{
	if(!isGood()) return false;
	command(NETWORK_CMD_CONFIG_SET_DOUBLE);
	sendString(key);
	ostringstream os;
	os << value;
	sendString(os.str());
	(*tcp()).flush();
	if(response()!=NETWORK_CODE_SUCCESS) return false;
	return true;
}



/** Helper functions */

void NetworkClient::command(unsigned char id)
{
	NetworkDataPacket cmd;
	cmd.id = id;
	*tcp() << cmd;
	(*tcp()).flush();
}

unsigned char NetworkClient::response()
{
	(*tcp()).sync();
	if(!isGood()) return NETWORK_CODE_UNKNOWN;
	NetworkDataPacket re;
	*tcp() >> re;
	if(!(*tcp())) return NETWORK_CODE_UNKNOWN;
	return re.id;
}


void NetworkClient::sendString(const string &word)
{
	*tcp()<<SHORT_TO_WORD_LOW(word.size())<<SHORT_TO_WORD_HIGH(word.size());
	for(int i=0;i<word.size();i++)
	{
		*tcp()<<word[i];
	}
}
const string NetworkClient::receiveString()
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

