/**
 * Motor basic class with safty and coasting functions
 */

#ifndef ERC_MOTOR_H
#define ERC_MOTOR_H
#include "../LocalModule.h"
#include "../DataPointer.h"
#include <sys/time.h>

namespace erc
{
	class Motor : public LocalModule
	{
	public:
		Motor(DebugStream& debug, libconfig::Setting &setting);
		virtual ~Motor();

		DataPointer getData(const unsigned char arg) const;
		bool setData(DataPointer pointer, const unsigned char arg);
		bool prepare();
		bool isGood() const;
		bool isReady();
		void close();

	protected:
		void run();
		virtual bool setValue(const unsigned char motor, const unsigned char speed) = 0;
		bool good;
		unsigned short int num_of_motor;
		ost::Mutex mutex;

	private:
		struct timeval lastupdate;
		DataPointer currentData;
	};
}

#endif
