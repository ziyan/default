#ifndef ERC_PHIDGETMC_H
#define ERC_PHIDGETMC_H
#include "Motor.h"
#include <phidget21.h>

namespace erc
{
	class PhidgetMC : public Motor
	{
	public:
		PhidgetMC(DebugStream& debug, libconfig::Setting &setting);
		~PhidgetMC();

	private:
		bool setValue(const unsigned char motor, const unsigned char speed);
		CPhidgetServoHandle phidget;
	};
}

#endif
