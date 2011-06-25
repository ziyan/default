/**
 * File Descriptor Timer
 */

#ifndef ERC_TIMEDRECV_H
#define ERC_TIMEDRECV_H

namespace erc
{
	int timedrecv(int seconds, int usecs, int file_descriptor);
}

#endif
