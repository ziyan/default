/**
 * Module wrapper for compass CMPS303
 */

#ifndef ERC_COMPASS_H
#define ERC_COMPASS_H

#include "../LocalModule.h"
#include "../DataPointer.h"

namespace erc
{
	class Compass: public LocalModule
	{
	public:
		Compass(DebugStream& debug, libconfig::Setting &setting);
		~Compass();

		bool isGood() const;
		void close();
		DataPointer getData(const unsigned char arg = 0) const;

	protected:
		void run();

	private:
		int fd;
		ost::Mutex mutex;
		bool good;
		DataPointer currentData;
    };
}

#endif
