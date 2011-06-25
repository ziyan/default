/**
 * Jaus command receiver
 */

#ifndef ERC_JAUS_H
#define ERC_JAUS_H

#include <cc++/socket.h>
#include "../LocalModule.h"

namespace erc
{
	class Jaus : public LocalModule
	{
	public:
		Jaus(DebugStream& debug, libconfig::Setting &setting);
		~Jaus();

		bool isGood() const;
		DataPointer getData(const unsigned char arg = 0) const;
		void close();

	protected:
		void run();

	private:
		ost::UDPSocket socket;
		bool good;
		DataPointer currentData;
	};
}

#endif
