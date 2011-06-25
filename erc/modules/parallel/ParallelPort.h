#ifndef ERC_PARALLELPORT_H
#define ERC_PARALLELPORT_H

#include "../LocalModule.h"

namespace erc
{
	class ParallelPort : public LocalModule
	{
	public:
		ParallelPort(DebugStream& debug, libconfig::Setting &setting);
		~ParallelPort();
		bool isGood() const;
		void close();
		DataPointer getData(const unsigned char arg = 0) const;
		bool setData(DataPointer pointer, const unsigned char arg);
	protected:
		virtual void run();
	private:
		DataPointer current;
		bool good;
	};
}

#endif
