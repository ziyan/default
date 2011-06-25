/**
 * PID loop for AI
 */


#include "PID.h"
#include "../daemon/Daemon.h"
#include "../modules/motor/MotorData.h"
#include "../modules/compass/CompassData.h"

using namespace std;
using namespace erc;

PID::PID(Daemon &daemon) :
	daemon(daemon),

	currentHeading(0),
	targetHeading(0),
	currentThrottle(0),
	targetThrottle(0),

	good(false)
{
	good = true;
}

PID::~PID()
{
}

void PID::run()
{
	double error = 0, last_error = 0, elapse = 0, correction = 0;
	double pGain = 0, iGain = 0, dGain = 0, iSum = 0, accel = 0, decel = 0;
	struct timeval lastupdate, currenttime;
	if(gettimeofday( &lastupdate,  NULL )<0) return;
	if(gettimeofday( &currenttime, NULL )<0) return;

	while(isGood())
	{
		//get current heading from compass
		Module* compass = daemon.getModule(daemon.getConfig<string>("control.pid.compass"));
		if(compass && compass->isGood())
		{
			currentHeading = (double)(((CompassData&)(*(compass->getData()))).direction)/10.0;
		}
		else
		{
			currentHeading = 0;//targetHeading;
		}

		error = targetHeading - currentHeading;
		if(error>180)  error -=360;
		if(error<-180) error +=360;
		
		pGain = (double)daemon.getConfig<double>("control.pid.p");
		iGain = (double)daemon.getConfig<double>("control.pid.i");
		dGain = (double)daemon.getConfig<double>("control.pid.d");
		accel = (double)daemon.getConfig<double>("control.pid.acceleration");
		decel = (double)daemon.getConfig<double>("control.pid.deceleration");
		
		//get the time interval
		if(gettimeofday( &currenttime,  NULL )<0) return;
		elapse = (double)(currenttime.tv_usec - lastupdate.tv_usec) + (double)(currenttime.tv_sec - lastupdate.tv_sec)*1000.0*1000.0;

		//set the update time
		if(gettimeofday( &lastupdate,  NULL )<0) return;



		//coasting
		if(currentThrottle < targetThrottle)
		{
			if(currentThrottle < 0)
			{
				currentThrottle += decel*elapse;
				if(currentThrottle>0) currentThrottle = 0;
			}
			else
			{
				currentThrottle += accel*elapse;
				if(currentThrottle>127) currentThrottle = 127;
			}
			//overflow
			if(currentThrottle > targetThrottle)
				currentThrottle = targetThrottle;
			
		}
		else if(currentThrottle > targetThrottle)
		{
			if(currentThrottle>0)
			{
				currentThrottle -= decel*elapse;
				if(currentThrottle<0) currentThrottle = 0;
			}
			else
			{
				currentThrottle -= accel*elapse;
				if(currentThrottle<-127) currentThrottle = -127;
			}
			//overflow
			if(currentThrottle < targetThrottle)
				currentThrottle = targetThrottle;
		}

		//integrate the error
		iSum+=error*elapse;

		//calculate the correction
		correction = pGain * error 
			+ iGain * iSum
			+ dGain * ((double)(error - last_error)/elapse);



		//output
		if(isGood()) output(correction, currentThrottle);

		//save last error
		last_error = error;

		Thread::sleep(10);
	}
}

void PID::final()
{
}

void PID::output(double steering, double throttle)
{
	//filter out NaN
	if(steering!=steering || throttle!=throttle) return;

	//turning limitation
	double turn_limit = daemon.getConfig<int>("control.pid.turn_limit");
	if(steering>turn_limit) steering = turn_limit;
	if(steering<-turn_limit) steering = -turn_limit;
	
	//calculate left and right motor speed
	int left = (int)(throttle + steering) + 127;
	int right= (int)(throttle - steering) + 127;

	//boundary check
	if(left<0) left = 0;
	if(left>255) left = 255;
	if(right<0) right = 0;
	if(right>255) right = 255;
	//daemon.debug << "PID: Motor value set: "<<left<<", "<<right<<endl;

	//get the motor
	Module *motor = daemon.getModule(daemon.getConfig<string>("control.pid.motor"));
	if(!(motor&&motor->isGood())) return;

	//get a copy of current motor speed
	DataPointer motor_value_ptr = motor->getData();
	if(motor_value_ptr.isNull()) return;

	//get the left and right motor id
	unsigned short int motor_left = daemon.getConfig<int>("control.pid.motor_left");
	unsigned short int motor_right= daemon.getConfig<int>("control.pid.motor_right");

	if(((MotorData&)(*motor_value_ptr)).num<motor_left+1) return;
	if(((MotorData&)(*motor_value_ptr)).num<motor_right+1) return;

	//set the motor values
	((MotorData&)(*motor_value_ptr)).value[motor_left] = left;
	((MotorData&)(*motor_value_ptr)).value[motor_right]= right;

	motor->setData(motor_value_ptr);
}

void PID::setHeading(double heading)
{
	targetHeading = currentHeading + heading;
}

void PID::setThrottle(char speed)
{
	targetThrottle = (double)(int)speed;
}

void PID::stop()
{
	targetHeading = currentHeading;
	currentThrottle = 0;
	targetThrottle = 0;
	output(0,0);
}

double PID::getCurrentHeading() const
{
	return currentHeading;
}

bool PID::isGood() const
{
	return good;
}

void PID::close()
{
	good = false;
	targetHeading = currentHeading;
	currentThrottle = 0;
	targetThrottle = 0;
	output(0,0);
}


