/**
 * NetworkDataPacket contains command and response, sent as a header
 */

#include "NetworkDataPacket.h"
#include <cc++/thread.h>

using namespace std;
using namespace erc;
using namespace ost;

istream& erc::operator>>(istream &is, NetworkDataPacket &p)
{
	//STX
	char stx;
	while(is.get(stx)&&stx!=NetworkDataPacket::STX);  //Possibly unsafe, false STX can exist!

	//id
	char id;
	is.get(id);
	if(!is) return is;
	p.id = id;

	return is;
}
ostream& erc::operator<<(ostream &os, const NetworkDataPacket &p)
{
	//STX
	os << NetworkDataPacket::STX;
	//data
	os << p.id;

	return os;
}
