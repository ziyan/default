#include "CameraFrame.h"
#include "../../common/define.h"
#include "../ModulesDefine.h"

using namespace std;
using namespace erc;

CameraFrame::CameraFrame() :
	width(0),
	height(0),
	channel(0),
	frame(0)
{
}

CameraFrame::~CameraFrame()
{
	delete [] frame;
}

unsigned char CameraFrame::getType() const
{
	return DATATYPE_CAMERAFRAME;
}

istream &erc::operator>>(istream &is, CameraFrame &frame)
{
	short int width,height,channel;
	char *data = 0;
	char l,h;
	//width
	is.get(l);
	is.get(h);
	width = WORD_TO_SHORT(l,h);
	//height
	is.get(l);
	is.get(h);
	height = WORD_TO_SHORT(l,h);
	//channel
	is.get(l);
	is.get(h);
	channel = WORD_TO_SHORT(l,h);
	//frame
	if(!is) return is;
	if(width*height*channel>0)
	{
		data = new char[width*height*channel];
		for(int i=0;i<width*height*channel;i++)
			is.get(data[i]);
		if(!is)
		{
			delete [] data;
			return is;
		}
	}
	frame.width = width;
	frame.height = height;
	frame.channel = channel;
	delete [] frame.frame;
	if(data)
		frame.frame = (unsigned char*)data;
	else
		frame.frame = 0;
	return is;
}
ostream &erc::operator<<(ostream &os, const CameraFrame &frame)
{
	os << SHORT_TO_WORD_LOW(frame.width) << SHORT_TO_WORD_HIGH(frame.width);
	os << SHORT_TO_WORD_LOW(frame.height) << SHORT_TO_WORD_HIGH(frame.height);
	os << SHORT_TO_WORD_LOW(frame.channel) << SHORT_TO_WORD_HIGH(frame.channel);
	if(frame.frame)
	{
		for(int i=0;i<frame.width*frame.height*frame.channel;i++)
			os << frame.frame[i];
	}
	return os;
}

