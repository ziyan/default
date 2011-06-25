/**
 * Module that runs locally
 */

 
#ifndef ERC_LOCALMODULE_H
#define ERC_LOCALMODULE_H

#include <string>
#include <cc++/thread.h>
#include <libconfig.h++>

#include "../common/DebugStream.h"
#include "DataPointer.h"

namespace erc
{
	/**
	 * A base class for local modules
	 */
	class LocalModule : public ost::Thread
	{
	public:
		/**
		 * Constructor
		 * @param args the arguments for start up
		 */
		LocalModule(DebugStream& debug, libconfig::Setting &setting);
		/**
		 * Destructor
		 */
		virtual ~LocalModule();

		/**
		 * Procedure done prior to start()
		 */
		virtual bool prepare();
		/**
		 * Error flag
		 */
		virtual bool isGood() const;
		/**
		 * Does necessary steps to make the system ready for use after construction
		 * Called after start()
		 */
		virtual bool isReady();
		/**
		 * Close the module
		 */
		virtual void close();
		/**
		 * Get data
		 */
		virtual DataPointer getData(const unsigned char arg = 0) const;
		/**
		 * Set data
		 */
		virtual bool setData(DataPointer pointer, const unsigned char arg = 0);

	protected:
		virtual void run();
		virtual void final();

		DebugStream& debug;
		libconfig::Setting &setting;
	};
}

#endif
