#include "CompassData.h"
#include "../../common/define.h"
#include "../ModulesDefine.h"

using namespace std;
using namespace erc;

unsigned char CompassData::getType() const
{
	return DATATYPE_COMPASSDATA;
}

istream &erc::operator>>(istream &is, CompassData &data)
{
	char l,h;
	is.get(l);
	is.get(h);
	if(!is) return is;
	data.direction = WORD_TO_SHORT(l,h);
	return is;
}
ostream &erc::operator<<(ostream &os, const CompassData &data)
{
	os << SHORT_TO_WORD_LOW(data.direction) << SHORT_TO_WORD_HIGH(data.direction) << endl;
	return os;
}
