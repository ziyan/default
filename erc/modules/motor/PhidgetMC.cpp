#include "PhidgetMC.h"
#include "MotorData.h"

#define NUM_OF_MOTOR 4
#define SERVO_SCALE 100
#define SERVO_OFFSET 70
#define SERVO_VALUE(x) (((double)(int)(unsigned char)x/255.0)*SERVO_SCALE+SERVO_OFFSET)

using ost::Mutex;
using namespace erc;
using namespace std;

PhidgetMC::PhidgetMC(DebugStream& debug, libconfig::Setting &setting) : Motor(debug,setting)
{
	good = false;
	num_of_motor = NUM_OF_MOTOR;
	CPhidgetServo_create(&phidget);
	if(!CPhidget_open((CPhidgetHandle)phidget, -1)) good = true;
}

PhidgetMC::~PhidgetMC()
{
	CPhidget_close((CPhidgetHandle)phidget);
	CPhidget_delete((CPhidgetHandle)phidget);

}

bool PhidgetMC::setValue(const unsigned char motor, const unsigned char value)
{
	if (!good) return false;
	if (motor >= NUM_OF_MOTOR) return false;
	mutex.enterMutex();
	if (CPhidgetServo_setMotorPosition(phidget, motor, SERVO_VALUE(value)))
		  good = false;
	mutex.leaveMutex();
	return good;
}

