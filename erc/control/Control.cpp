/**
 * System controller
 */

#include "Control.h"
#include "../daemon/Daemon.h"
#include "../modules/Modules.h"
#include "../common/Point.h"
#include "../common/define.h"

using namespace erc;
using namespace std;

Control::Control(Daemon &daemon)
 : daemon(daemon),pid(daemon),good(false)
{
	if(pid.isGood())
		pid.start();
	good = true;
}

Control::~Control()
{
	pid.close();
}

bool Control::isGood() const
{
	return good && pid.isGood();
}

void Control::stop()
{
	good = false;
	Thread::sleep(100);
	//just to make sure that the loop is no longer running
	mutex.enterMutex();
	mutex.leaveMutex();
}

void Control::run()
{

	while(isGood())
	{
		mutex.enterMutex();
		//control loop start

		//check for joystick control
		if(!joystick())
		{
			if(!jaus())
			{
				//now go into ai mode
				autonomous();
			}
			else
			{
				//stop
				pid.stop();
			}
		}
		//control loop stop
		Thread::sleep(5);
		mutex.leaveMutex();
	}
	good = false;
	pid.stop();
}

void Control::final()
{
}

bool Control::parallel_port_data(unsigned char data, bool value)
{
	if(data>=8) return false;
	Module* relay = daemon.getModule(daemon.getConfig<string>("control.parallel_port.parallel_port"));
	if(!relay || !(relay->isGood())) return false;
	DataPointer ptr = relay->getData();
	((ParallelPortData&)(*ptr)).data[data] = value;
	return relay->setData(ptr);
}
bool Control::parallel_port_data(unsigned char data)
{
	if(data>=8) return false;
	Module* relay = daemon.getModule(daemon.getConfig<string>("control.parallel_port.parallel_port"));
	if(!relay || !(relay->isGood())) return false;
	DataPointer ptr = relay->getData();
	return ((ParallelPortData&)(*ptr)).data[data];
}
bool Control::parallel_port_status(unsigned char status)
{
	if(status>=5) return false;
	Module* relay = daemon.getModule(daemon.getConfig<string>("control.parallel_port.parallel_port"));
	if(!relay || !(relay->isGood())) return false;
	DataPointer ptr = relay->getData();
	return ((ParallelPortData&)(*ptr)).status[status];
}




bool Control::jaus()
{
	//get the jaus
	Module* jaus = daemon.getModule(daemon.getConfig<string>("control.jaus.jaus"));
	if(!jaus || !(jaus->isGood())) return false;
	
	//get the data
	DataPointer ptr = jaus->getData();
	if(ptr.isNull()) return false;
	
	//turn the switch
	if(((JausData&)(*ptr)).warning)
	{
		parallel_port_data(daemon.getConfig<int>("control.parallel_port.warning_device"),true);
	}
	else
	{
		parallel_port_data(daemon.getConfig<int>("control.parallel_port.warning_device"),false);
	}
	
	if(!((JausData&)(*ptr)).mobility)
	{
		//stop
		return true;
	}
	return false;
}

bool Control::joystick()
{
	//get the joystick
	Module* joystick = daemon.getModule(daemon.getConfig<string>("control.joystick.joystick"));

	if(!joystick || !(joystick->isGood())) return false;

	//get the data
	DataPointer ptr = joystick->getData();
	if(ptr.isNull()) return false;

	
	//check for warning device
	if(((JoyStickData&)(*ptr)).button[daemon.getConfig<int>("control.joystick.warning_button")])
	{
		parallel_port_data(daemon.getConfig<int>("control.parallel_port.warning_device"),true);
	}
	else
	{
		parallel_port_data(daemon.getConfig<int>("control.parallel_port.warning_device"),false);
	}
	
	//check for stop
	if(((JoyStickData&)(*ptr)).button[daemon.getConfig<int>("control.joystick.stop_button")])
	{
		pid.stop();
		return true;
	}

	//check for ai mode
	if(((JoyStickData&)(*ptr)).button[daemon.getConfig<int>("control.joystick.ai_button")])
		return false;
		
	

	int speedlimit = 0;

	//check for speed
	if(((JoyStickData&)(*ptr)).button[daemon.getConfig<int>("control.joystick.speed_button_1")])
		speedlimit = daemon.getConfig<int>("control.joystick.speed_1");
	else if(((JoyStickData&)(*ptr)).button[daemon.getConfig<int>("control.joystick.speed_button_2")])
		speedlimit = daemon.getConfig<int>("control.joystick.speed_2");
	else if(((JoyStickData&)(*ptr)).button[daemon.getConfig<int>("control.joystick.speed_button_3")])
		speedlimit = daemon.getConfig<int>("control.joystick.speed_3");
	else if(((JoyStickData&)(*ptr)).button[daemon.getConfig<int>("control.joystick.speed_button_4")])
		speedlimit = daemon.getConfig<int>("control.joystick.speed_4");
	else
		speedlimit = 0;

	//throttle and steering
	int throttle =    ((JoyStickData&)(*ptr)).axis[daemon.getConfig<int>("control.joystick.throttle_axis")];
	double steering = ((JoyStickData&)(*ptr)).axis[daemon.getConfig<int>("control.joystick.steering_axis")];

	if(((JoyStickData&)(*ptr)).axis[daemon.getConfig<int>("control.joystick.straight_axis")]!=0)
		throttle = ((JoyStickData&)(*ptr)).axis[daemon.getConfig<int>("control.joystick.straight_axis")];
	if(((JoyStickData&)(*ptr)).axis[daemon.getConfig<int>("control.joystick.turn_axis")]!=0)
		steering = ((JoyStickData&)(*ptr)).axis[daemon.getConfig<int>("control.joystick.turn_axis")];

	//scaling the throttle and steering
	throttle = (int)(((double)-throttle)*((double)speedlimit/127.0));
	//safer turning
	steering = steering * ((double)daemon.getConfig<int>("control.joystick.turn_limit")/127.0);

	//deadman switch
	if(!((JoyStickData&)(*ptr)).button[daemon.getConfig<int>("control.joystick.deadman_switch")])
	{
		throttle = 0;
		steering = 0;
	}

	//tell the pid to turn
	if(throttle> 127) throttle = 127;
	if(throttle<-127) throttle = -127;
	pid.setThrottle((char)throttle);
	daemon.debug<< "Control: joystick set throttle = "<<throttle<<endl;
	pid.setHeading(steering);
	daemon.debug<< "Control: joystick set heading = "<<steering<<endl;

	return true;
}


void Control::autonomous()
{
	pid.setHeading(obstacle_avoidance(waypoint()));
}

double Control::obstacle_avoidance(double target)
{
	//object avoidance mode
	Module *lidar = daemon.getModule(daemon.getConfig<string>("control.obstacle_avoidance.lidar"));
	if(!lidar || !(lidar->isGood())) return 0.0;
	DataPointer ptr = lidar->getData();
	if(ptr.isNull()) return 0.0;
	
	//if no path in front, turn around
	double heading = 180;


	if(((LidarData&)(*ptr)).min>=daemon.getConfig<int>("control.obstacle_avoidance.limit"))
	{
		//nothing in view go straight
		heading = target;
	}
	else
	{
	
		double best_angle = 180.0;

		bool gap_started = false;
		short int start_angle = 180; //right 90 degrees

		unsigned short int roi_offset = daemon.getConfig<int>("control.obstacle_avoidance.offset");
		double buffer_angle = RAD_TO_DEG(asin((double)(daemon.getConfig<int>("robot.width")/2+daemon.getConfig<int>("robot.buffer")) / (double)(((LidarData&)(*ptr)).min+roi_offset)));
		daemon.debug << "Control: object avoidance buffer angle = "<<buffer_angle<<endl;

		for(int i=0;i<LIDAR_DATA_LENGTH;i++)
		{
		
			if(((LidarData&)(*ptr)).distance[i] > (((LidarData&)(*ptr)).min+roi_offset) && !gap_started)
			{
				//start angle
				start_angle = LIDAR_DATA_LENGTH/2 - i;
				gap_started = true;
			}
			else if((((LidarData&)(*ptr)).distance[i] < (((LidarData&)(*ptr)).min+roi_offset) || (i+1)==LIDAR_DATA_LENGTH) && gap_started)
			{
				//stop angle
				short int stop_angle = LIDAR_DATA_LENGTH/2 - i;
				Point startp, endp;
				startp.y = (short int)(cos(DEG_TO_RAD((double)start_angle/2)) * (((LidarData&)(*ptr)).min+roi_offset));
				startp.x = (short int)(sin(DEG_TO_RAD((double)start_angle/2)) * (((LidarData&)(*ptr)).min+roi_offset));
				endp.y   = (short int)(cos(DEG_TO_RAD((double)stop_angle/2))  * (((LidarData&)(*ptr)).min+roi_offset));
				endp.x   = (short int)(sin(DEG_TO_RAD((double)stop_angle/2))  * (((LidarData&)(*ptr)).min+roi_offset));
				double gap = endp-startp;
				if(gap > daemon.getConfig<int>("robot.width"))
				{
					//get mid angle;
					daemon.debug<<"Lidar: good gap from "
						<<(double)start_angle/2<<" to "
						<<(double)stop_angle/2
						<<" at "<<(((LidarData&)(*ptr)).min+roi_offset)<<"mm away"<<endl;


					double angle = (double)(start_angle+stop_angle)/4;
					if((double)start_angle/2 - buffer_angle < (double)stop_angle/2 + buffer_angle)
					{
						//the gap is too small so that we choose the mid angle
					}
					else if(target < (double)start_angle/2 - buffer_angle
						&& target > (double)stop_angle/2 + buffer_angle)
					{
						//the gap allows up to go straight towards target
						angle = target;
					}
					else if(target <= (double)stop_angle/2 + buffer_angle)
					{
						angle = (double)stop_angle/2 + buffer_angle;
					}
					else
					{
						angle = (double)start_angle/2 - buffer_angle;
					}


					if(fabs(angle - target)<best_angle)
					{
						best_angle = fabs(angle - target);
						heading = angle;
					}
					//daemon.debug << "Control: object avoidance angle = "<< angle << endl;
				}
				gap_started = false;
			}
		
		}
	}
	pid.setHeading(heading);
	daemon.debug << "Control: target angle = "<< target<< ", heading = "<< heading << endl;

	//throttle setting
	int throttle = 0;
	int object_avoidance_danger = daemon.getConfig<int>("control.obstacle_avoidance.danger");

	//see if need to stop
	if(fabs(heading)<90 && ((LidarData&)(*ptr)).min>object_avoidance_danger)
	{
		throttle = daemon.getConfig<int>("control.obstacle_avoidance.throttle");
		int object_avoidance_clear = daemon.getConfig<int>("control.obstacle_avoidance.clear");
		//see if clear to drive at normal speed
		if(((LidarData&)(*ptr)).min <= object_avoidance_clear)
		{
			//objects are close, slow down
			throttle = (int)((double)(((LidarData&)(*ptr)).min-object_avoidance_danger)
					/(double)(object_avoidance_clear-object_avoidance_danger)
					*(double)(throttle-10))+10;
		}
	}
	pid.setThrottle(throttle);
	return heading;
}

double Control::waypoint()
{
	//way point navigation mode
	Module *gps = daemon.getModule(daemon.getConfig<string>("control.waypoint.gps"));
	if(!gps || !(gps->isGood())) return 0.0;
	DataPointer ptr = gps->getData();
	if(ptr.isNull()) return 0.0;
	
	//current waypoint
	GpsData& waypoint_current = ((GpsData&)(*ptr));
	daemon.debug<<"Control: current waypoint is "<<waypoint_current.latitude<<", "<<waypoint_current.longitude<<endl;

	//get target sequence number
	int index = daemon.getConfig<int>("control.waypoint.index");
	
	//get target waypoint
	GpsData waypoint_target;
	double waypoint_range;
	
	try
	{
		ostringstream ss;
		ss << index;
		waypoint_target.latitude = (double)daemon.config.lookup("control.waypoint.point"+ss.str()+".latitude");
		waypoint_target.longitude = (double)daemon.config.lookup("control.waypoint.point"+ss.str()+".longitude");
		waypoint_range = (double)daemon.config.lookup("control.waypoint.point"+ss.str()+".range");
	}
	catch(...)
	{
		//found a bad waypoint, skip it
		index++;
		if(index<=daemon.getConfig<int>("control.waypoint.target"))
		{
			daemon.setConfig<int>("control.waypoint.index",index);
		}
		return 0.0;
	}
	waypoint_target.updateUTM();
	daemon.debug<<"Control: target waypoint is "<<waypoint_target.latitude<<", "<<waypoint_target.longitude
		<<" with range "<<waypoint_range<<endl;


	//check if target reached
	daemon.debug<<"Control: waypoint distance is "<<(waypoint_target-waypoint_current)<<endl;
	if((waypoint_target-waypoint_current)<waypoint_range)
	{
		index++;
		if(index>daemon.getConfig<int>("control.waypoint.target"))
		{
			return 0.0;
		}
		else
		{
			daemon.setConfig<int>("control.waypoint.index",index);
		}
	}

	//calculate direction
	double direction = waypoint_current/waypoint_target;
	double current_heading = pid.getCurrentHeading();
	//correction
	if(current_heading>180) current_heading -= 360.0;
	if(current_heading<-180) current_heading += 360.0;
	return direction - current_heading;
}

double Control::image_process()
{
	//get camera device
	Module *camera = daemon.getModule(daemon.getConfig<string>("control.image_process.camera"));
	if(!camera || !(camera->isGood())) return 0.0;
	DataPointer ptr = camera->getData();
	if(ptr.isNull()) return 0.0;
	
	//image process
	CameraData& data = ((CameraData&)(*ptr));
	if(data.state == CAMERA_NO_LINES)
	{
		//oops no line detected
		return 0.0;
	}
	return data.center.theta();
}

