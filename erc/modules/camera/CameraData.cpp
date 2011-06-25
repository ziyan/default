#include "CameraData.h"
#include "../../common/define.h"
#include "../ModulesDefine.h"


using namespace std;
using namespace erc;


unsigned char CameraData::getType() const
{
	return DATATYPE_CAMERADATA;
}

istream &erc::operator>>(istream &is, CameraData &data)
{
	short int width, height;
	Line left, center, right;
	unsigned char state;
	char l,h;
	//width
	is.get(l);
	is.get(h);
	width = WORD_TO_SHORT(l,h);
	//height;
	is.get(l);
	is.get(h);
	height = WORD_TO_SHORT(l,h);
	//lines
	is >> left >> center >> right;
	//state
	is.get(l);
	state = (unsigned char)l;
	if(!is) return is;
	data.width = width;
	data.height = height;
	data.left = left;
	data.center = center;
	data.right = right;
	data.state = state;
        return is;
}
ostream &erc::operator<<(ostream &os, const CameraData &data)
{
	os << SHORT_TO_WORD_LOW(data.width) << SHORT_TO_WORD_HIGH(data.width);
	os << SHORT_TO_WORD_LOW(data.height)<< SHORT_TO_WORD_HIGH(data.height);
	os << data.left << data.center << data.right;
	os << data.state;
        return os;
}

