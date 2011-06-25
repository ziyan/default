#include "ParallelPort.h"
#include "ParallelPortData.h"
#include "../ModulesDefine.h"
#include <sys/io.h>


using namespace erc;
using namespace std;
using namespace ost;

#define BASEPORT 0x378

ParallelPort::ParallelPort(DebugStream& debug, libconfig::Setting &setting)
	: LocalModule(debug,setting), good(false), current(new ParallelPortData)
{
	if(ioperm(BASEPORT, 3, 1)) return;
	good = true;
}

ParallelPort::~ParallelPort()
{
}

bool ParallelPort::isGood() const
{
	return good;
}

void ParallelPort::close()
{
	ioperm(BASEPORT, 3, 0);
}

DataPointer ParallelPort::getData(const unsigned char arg) const
{
	return current;
}

bool ParallelPort::setData(DataPointer pointer, const unsigned char arg)
{
	if(!pointer.isNull() && pointer->getType()==DATATYPE_PARALLELPORTDATA)
	{
		int data = 0;
		for(int i=0;i<8;i++)
		{
			((ParallelPortData&)(*current)).data[i] = ((ParallelPortData&)(*pointer)).data[i]; 
			data |= ((ParallelPortData&)(*current)).data[i] << i;
		}
		outb(data, BASEPORT);
		return true;
	}
	return false;
}

void ParallelPort::run()
{
	while(isGood())
	{
		int data = 0;
		for(int i=0;i<8;i++)
		{
			data |= ((ParallelPortData&)(*current)).data[i] << i;
		}
		outb(data, BASEPORT);
		data = inb(BASEPORT);
		for(int i=0;i<8;i++)
		{
			((ParallelPortData&)(*current)).data[i] = (data >> i) & 0x1;
		}
		int status = (inb(BASEPORT+1) ^ 120) >> 3;
		for(int i=0;i<5;i++)
		{
			((ParallelPortData&)(*current)).status[i] = (status >> i) & 0x1;
		} 
		Thread::sleep(500);
	}
}


