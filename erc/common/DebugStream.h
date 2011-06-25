/**
 * debugging stream for erc
 */

#ifndef ERC_DEBUGSTREAM_H
#define ERC_DEBUGSTREAM_H

#include <iostream>
#include <vector>
#include <string>

namespace erc
{
	class DebugStream
	{
	public:
		DebugStream();
		~DebugStream();

		void reg(std::ostream*);
		void dereg(std::ostream*);

		template<class T>
		DebugStream& operator<<(const T &in)
		{
			std::vector<std::ostream*>::iterator i;
			for(i=listeners.begin();i!=listeners.end();++i)
			{
				if(!(*i)) continue;
				(**i)<<in;
			}
			return *this;
		}
		template<class T>
		DebugStream& operator<<(const T* &in)
		{
			std::vector<std::ostream*>::iterator i;
			for(i=listeners.begin();i!=listeners.end();++i)
			{
				if(!(*i)) continue;
				(**i)<<in;
			}
			return *this;
		}
		DebugStream& operator<<(std::ostream& (*)(std::ostream&));
		void flush();
		

		
	private:
		std::vector<std::ostream*> listeners;
	};
}

#endif
 
