/**
 * File Descriptor Timer
 */

#include "timedrecv.h"
#include <cstdlib>

int erc::timedrecv(int secs, int usecs, int fd)
{
	fd_set input;
	struct timeval timer;
	FD_ZERO(&input);
	FD_SET(fd, &input);
	timer.tv_sec = secs;
	timer.tv_usec = usecs;	
	return select(fd+1, &input, NULL, NULL, &timer);
}
