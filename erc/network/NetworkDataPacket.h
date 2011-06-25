/**
 * NetworkDataPacket contains command and response, sent as a header
 */

#ifndef ERC_DATAPACKET_H
#define ERC_DATAPACKET_H
#include <iostream>

namespace erc
{
	struct NetworkDataPacket
	{
		static const unsigned char STX = 0x02;
		unsigned char id;

		friend std::ostream& operator<<(std::ostream &os, const NetworkDataPacket &p);
		friend std::istream& operator>>(std::istream &is, NetworkDataPacket &p);
	};
}

#endif
