#include "JoyStickData.h"
#include "../ModulesDefine.h"

using namespace std;
using namespace erc;

unsigned char JoyStickData::getType() const
{
	return DATATYPE_JOYSTICKDATA;
}

JoyStickData::JoyStickData()
 : num_of_buttons(0),num_of_axes(0),button(0),axis(0)
{
}

JoyStickData::~JoyStickData()
{
	if(axis) delete [] axis;
	if(button) delete [] button;
}

istream &erc::operator>>(istream &is, JoyStickData &data)
{
	char b = 0, a = 0;
	is.get(b);
	is.get(a);
	if(!is) return is;
	int buttons = (int)(unsigned char)b;
	int axes = (int)(unsigned char)a;
	char *button = 0;
	char *axis = 0;
	if(buttons>0)
	{
		button = new char[buttons];
		for(int i=0;i<(int)buttons;i++) is.get(button[i]);
	}
	if(axes>0)
	{
		axis = new char[axes];
		for(int i=0;i<(int)axes;i++) is.get(axis[i]);
		
	}

	if(!is)
	{
		if(button) delete [] button;
		if(axis) delete [] axis;
		return is;
	}
	data.num_of_buttons = buttons;
	data.num_of_axes = axes;
	data.button = button;
	data.axis = axis;
	return is;
}
ostream &erc::operator<<(ostream &os, const JoyStickData &data)
{
	os << data.num_of_buttons << data.num_of_axes;
	for(int i = 0;i<data.num_of_buttons;i++) os << data.button[i];
	for(int i = 0;i<data.num_of_axes;i++) os << data.axis[i];
	return os;
}
