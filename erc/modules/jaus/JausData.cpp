#include "JausData.h"
#include "../ModulesDefine.h"

using namespace erc;
using namespace std;

unsigned char JausData::getType() const
{
	return DATATYPE_JAUSDATA;
}

istream &erc::operator>>(istream &is, JausData &data)
{
	char mobility, warning;
	is >> mobility >> warning;
	if(!is) return is;
	data.mobility = (mobility == 't');
	data.warning = (warning == 't');
	return is;
}
ostream &erc::operator<<(ostream &os, const JausData &data)
{
	os << (data.mobility?'t':'f') << (data.warning?'t':'f');
	return os;
}
