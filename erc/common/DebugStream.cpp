/**
 * debugging stream for erc
 */

#include "DebugStream.h"
#include <cc++/thread.h>

using namespace std;
using namespace erc;
using namespace ost;

static Mutex mutex;

DebugStream::DebugStream() :
	listeners(0)
{
}

DebugStream::~DebugStream()
{
}

void DebugStream::reg(ostream* out)
{
	if(!out) return;
	if(!(*out)) return;
	vector<ostream*>::iterator i;
	for(i=listeners.begin();i!=listeners.end();++i)
	{
		if(*i==out) return;
	}
	listeners.push_back(out);
}
void DebugStream::dereg(ostream* out)
{
	vector<ostream*>::iterator i;
	for(i=listeners.begin();i!=listeners.end();++i)
	{
		if(*i==out)
		{
			listeners.erase(i);
			return;
		}
	}
}

DebugStream& DebugStream::operator<<(ostream& (*f)(ostream&))
{
	mutex.enterMutex();
	vector<ostream*>::iterator i;
	for(i=listeners.begin();i!=listeners.end();++i)
	{
		if(!(*i)) continue;
		(**i)<<f;
	}
	mutex.leaveMutex();
	return *this;
}

void DebugStream::flush()
{
	mutex.enterMutex();
	vector<ostream*>::iterator i;
	for(i=listeners.begin();i!=listeners.end();++i)
	{
		if(!(*i)) continue;
		(**i).flush();
	}
	mutex.leaveMutex();
}



