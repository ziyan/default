/**
 * Motor basic class with safty and coasting functions
 */

#include "Motor.h"
#include "MotorData.h"
#include "../ModulesDefine.h"

#define MOTOR_SET_VALUE_TIMEOUT 500

using namespace erc;
using namespace std;

Motor::Motor(DebugStream& debug, libconfig::Setting &setting) :
	LocalModule(debug,setting),
	good(false),
	currentData(new MotorData),
	num_of_motor(0)
{
}

Motor::~Motor()
{
}

bool Motor::prepare()
{
	if(num_of_motor<=0) return false;
	((MotorData&)(*currentData)).num = num_of_motor;
	((MotorData&)(*currentData)).value = new unsigned char[num_of_motor];

	for(int i=0;i<num_of_motor;i++)
	{
		((MotorData&)(*currentData)).value[i] = 127;
	}

	return true;
}

DataPointer Motor::getData(const unsigned char arg) const
{
	//copy the current data
	DataPointer ptr(new MotorData);
	
	((MotorData&)(*ptr)).num = num_of_motor;
	((MotorData&)(*ptr)).value = new unsigned char[num_of_motor];

	for(int i=0;i<num_of_motor;i++)
	{
		((MotorData&)(*ptr)).value[i] = ((MotorData&)(*currentData)).value[i];
	}
	return ptr;
}

bool Motor::setData(DataPointer pointer, const unsigned char arg)
{
	if(pointer.isNull()) return false;
	if(pointer->getType()!=DATATYPE_MOTORDATA) return false;
	MotorData &data = (MotorData&)(*pointer);
	for(int i=0;i<data.num&&i<((MotorData&)(*currentData)).num;i++)
	{
		((MotorData&)(*currentData)).value[i] = data.value[i];
		setValue(i,data.value[i]);
	}
	gettimeofday( &lastupdate, NULL );
	return true;
}

bool Motor::isGood() const
{
	return good;
}

bool Motor::isReady()
{
	return good;
}

void Motor::run()
{
	Thread::sleep(1000);
	gettimeofday( &lastupdate, NULL );
	debug<<"Motor: thread started."<<endl;
	while(isGood())
	{
		//check for timeout
		struct timeval currenttime;
		if(gettimeofday( &currenttime, NULL )<0) return;
		int elapse = (int)((double)(currenttime.tv_usec - lastupdate.tv_usec)/1000) + (currenttime.tv_sec - lastupdate.tv_sec)*1000;

		if(elapse >= MOTOR_SET_VALUE_TIMEOUT)
		{
			debug<<"Motor: set value timed out. stopping motors. "<<endl;
			for(int i=0;i<((MotorData&)(*currentData)).num;i++)
			{
				((MotorData&)(*currentData)).value[i] = 127;
				setValue(i,127);
			}
			gettimeofday( &lastupdate, NULL );
		}
		Thread::sleep(10);
	}
	//when stop set the value into stop again
	for(int i=0;i<((MotorData&)(*currentData)).num;i++)
	{
		((MotorData&)(*currentData)).value[i] = 127;
		setValue(i,127);
	}
	good = false;
	debug<<"Motor: thread exited."<<endl;
}

void Motor::close()
{
	good = false;
	sleep(100);
	//when stop set the value into stop again
	debug<<"Motor: setting all motor to stop."<<endl;
	for(int i=0;i<((MotorData&)(*currentData)).num;i++)
	{
		((MotorData&)(*currentData)).value[i] = 127;
		setValue(i,127);
	}
}

