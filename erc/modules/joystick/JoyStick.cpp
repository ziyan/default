/**
 * JoyStick driver for erc
 */

#include <iostream>
#include <linux/joystick.h>
#include "JoyStick.h"
#include "JoyStickData.h" 

using namespace erc;
using namespace std;
using namespace ost;

JoyStick::JoyStick(DebugStream& debug, libconfig::Setting &setting) :
	LocalModule(debug,setting),
	fd(-1),
	currentData(new JoyStickData),
	good(false)
{
	if((fd=open(setting["device"],O_RDONLY))<0) return;
	unsigned short int buttons, axes;
	if(ioctl(fd,JSIOCGBUTTONS,&buttons)!=0) return;
	if(ioctl(fd,JSIOCGAXES,&axes)!=0) return;
	((JoyStickData&)(*currentData)).num_of_buttons = buttons;
	((JoyStickData&)(*currentData)).num_of_axes = axes;

	((JoyStickData&)(*currentData)).axis = new char[axes];
	((JoyStickData&)(*currentData)).button = new char[buttons];
	good = true;
}

JoyStick::~JoyStick()
{
}

bool JoyStick::isGood() const
{
	return good && fd>0;
}

DataPointer JoyStick::getData(const unsigned char arg) const
{
	return currentData;
}

void JoyStick::run()
{
	struct js_event js;
	js.type = JS_EVENT_INIT;

	while(isGood())
	{
		if (read(fd, &js, sizeof(struct js_event)) != sizeof(struct js_event)) break;
		switch (js.type & ~JS_EVENT_INIT)
		{
			case JS_EVENT_AXIS:
				((JoyStickData&)(*currentData)).axis[js.number] = (char)((double)(js.value)/(258));
				debug<<"JoyStick: Axis "<<(int)js.number<<" changed to "<<(int)(char)((double)(js.value)/(258))<<"."<<endl;
				break;
			case JS_EVENT_BUTTON:
				((JoyStickData&)(*currentData)).button[js.number] = js.value;
				debug<<"JoyStick: Button "<<(int)js.number<<" "<<(js.value?"pressed.":"released.")<<endl;
				break;
		}
	}
	good = false;
}

void JoyStick::close()
{
	good = false;
}

