/**
 * Module wrapper for lidar LMS200
 */

#ifndef ERC_LIDAR_H
#define ERC_LIDAR_H

#include <SerialStream.h>
#include "../LocalModule.h"

namespace erc
{
	class Lidar : public LocalModule
	{
	public:
		Lidar(DebugStream& debug, libconfig::Setting &setting);
		~Lidar();

		bool isGood() const;
		DataPointer getData(const unsigned char arg = 0) const;
		void close();

	protected:
		void run();

	private:
		bool isConnected();

		DataPointer currentData;
		LibSerial::SerialStream *stream;
		ost::Mutex mutex;
		bool good;
	};
}

#endif
