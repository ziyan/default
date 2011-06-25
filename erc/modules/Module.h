/** 
 * Wrapper for modules
 */

#ifndef ERC_MODULE_H
#define ERC_MODULE_H

#include <string>
#include <libconfig.h++>

#include "../common/DebugStream.h"
#include "DataPointer.h"
#include "RemoteModule.h"
#include "LocalModule.h"

namespace erc
{
	/**
	 * A wrapper for modules
	 */
	class Module
	{
	public:
		/**
		 * Constructor
		 * @param name the name of this module
		 * @param type the type of this module
		 * @param config the configuration for starting up
		 */
		Module(DebugStream &debug, const std::string name, const unsigned char type, libconfig::Setting &setting);
		~Module();

		bool isGood() const;
		DataPointer getData(const unsigned char arg = 0) const;
		bool setData(DataPointer pointer, const unsigned char arg = 0);

		const std::string name;
		const unsigned char type;

	private:
		libconfig::Setting &setting;
		DebugStream &debug;
		LocalModule *local;
		RemoteModule *remote;
	};
}

#endif
