#ifndef ERC_LIDARDATAPACKET_H
#define ERC_LIDARDATAPACKET_H

#include <iostream>

namespace erc
{
	struct LidarDataPacket
	{
		static const unsigned char STX = 0x02;
		unsigned char addr;
		unsigned short int length;
		unsigned char *data;
		bool valid;
		LidarDataPacket();
		LidarDataPacket(const LidarDataPacket& other);
		~LidarDataPacket();
		LidarDataPacket &operator=(const LidarDataPacket& other);
		friend std::ostream& operator<<(std::ostream &os, const LidarDataPacket &p);
		friend std::istream& operator>>(std::istream &is, LidarDataPacket &p);
	};
}

#endif
