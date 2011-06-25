#ifndef ERC_GPS_H
#define ERC_GPS_H

#include <gps.h>
#include "../DataPointer.h"
#include "../LocalModule.h"

namespace erc 
{
	/**
	 * Gps data receiving module
	 */
	class Gps : public LocalModule
	{
	public:
		Gps(DebugStream& debug, libconfig::Setting &setting);
		~Gps();

		DataPointer getData(const unsigned char arg = 0) const;
		bool isReady();
		bool isGood() const;
		void close();


	protected:
		void run();

	private:
		bool good;
		struct gps_data_t* gps;
		ost::Mutex mutex;
		DataPointer currentData;
	};
}

#endif
