#ifndef ERC_SERIALMC_H
#define ERC_SERIALMC_H
#include "Motor.h"
#include <SerialStream.h>

namespace erc
{
	class SerialMC : public Motor
	{
	public:
		SerialMC(DebugStream& debug, libconfig::Setting &setting);
		~SerialMC();

	private:
		bool setValue(const unsigned char motor, const unsigned char speed);
		LibSerial::SerialStream *servo;
	};
}

#endif
