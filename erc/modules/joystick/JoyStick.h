/**
 * JoyStick driver for erc
 */

#ifndef ERC_JOYSTICK_H
#define ERC_JOYSTICK_H

#include "../LocalModule.h"

namespace erc
{
	class JoyStick : public LocalModule
	{
	public:
		JoyStick(DebugStream& debug, libconfig::Setting &setting);
		~JoyStick();

		bool isGood() const;
		DataPointer getData(const unsigned char arg = 0) const;
		void close();

	protected:
		void run();

	private:
		bool good;
		int fd;
		DataPointer currentData;
	};
}

#endif
