/**
 * PID loop for AI
 */

#ifndef ERC_PID_H
#define ERC_PID_H

#include <cc++/thread.h>
#include "../modules/Module.h"
#include "../modules/DataPointer.h"
#include "../common/DebugStream.h"

namespace erc
{
	class Daemon;
	class PID : public ost::Thread
	{
	public:
		PID(Daemon &daemon);
		~PID();
		double getCurrentHeading() const;
		void setHeading(double heading);
		void setThrottle(char throttle);
		void stop();
		bool isGood() const;
		void close();

	protected:
		void run();
		void final();
	private:
		void output(double steering, double throttle);

		bool good;

		Daemon &daemon;

		double currentHeading; //current heading
		double targetHeading; //target heading
		double currentThrottle; //current speed
		double targetThrottle; //target speed

	};
}

#endif

