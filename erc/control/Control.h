/**
 * System controller
 * This class would listen to joystick
 * and switch between autonomous mode and human operating mode
 */

#ifndef ERC_CONTROL_H
#define ERC_CONTROL_H
#include <cc++/thread.h>
#include "../common/DebugStream.h"
#include "PID.h"


namespace erc
{
	class Daemon;
	class Control : public ost::Thread
	{
	public:
		/**
		 * Constructor
		 */
		Control(Daemon &daemon);
		/**
		 * Destructor
		 */
		~Control();
		/**
		 * Error flag
		 */
		bool isGood() const;
		/**
		 * Set error flag, and wait for loop to quit
		 */
		void stop();

	protected:
		/**
		 * Called by thread scheduler
		 */
		void run();
		void final();

	private:
		/** PID Loop */
		PID pid;
		/** Reference to Daemon */
		Daemon &daemon;
		/** Mutex for loop */
		ost::Mutex mutex;
		/** Error flag */
		bool good;

		/** ParallelPort device */
		bool parallel_port_data(unsigned char data, bool value);
		bool parallel_port_data(unsigned char data);
		bool parallel_port_status(unsigned char status);
		
		/** Check for jaus mobility */
		bool jaus();
		/** Check for joystick control */
		bool joystick();
		/** Autonomous entry */
		void autonomous();
		/** obstacle_avoidance entry */
		double obstacle_avoidance(double target);
		/** Get GPS direction */
		double waypoint();
		/** Get Image Process direction */
		double image_process();
	};
}
#endif
