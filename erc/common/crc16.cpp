/**
 * crc16 checksum calculation
 */

#include "crc16.h"
#include <iostream>

using namespace std;

unsigned short int erc::crc16(unsigned char* msg, int start, int len) 
{
	unsigned short int CRC16_GEN_POL = 0x8005;
	unsigned short int crc16 = 0;
	unsigned char datum1 = 0;
	unsigned char datum2 = 0;
	for (int i=start;i<len;i++)
	{
		datum2 = datum1;
		datum1 = msg[i];
		if (crc16 & 0x8000)
		{
			crc16 - (crc16 & 0x7fff) << 1;
			crc16 = (crc16 ^ CRC16_GEN_POL);
		}
		else
			crc16 <<= 1;
		crc16 = crc16 ^ ((unsigned short)(datum1)|((unsigned short)(datum2)<<8));
	}
	return crc16;
}
