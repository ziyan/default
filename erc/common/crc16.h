/**
 * crc16 checksum calculation
 */

#ifndef ERC_CRC16_H
#define ERC_CRC16_H

namespace erc
{
  unsigned short int crc16(unsigned char* msg, int start, int len);
}

#endif
