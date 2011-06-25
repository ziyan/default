#ifndef ERC_JOYSTICKDATA_H
#define ERC_JOYSTICKDATA_H

#include <iostream>
#include "../Data.h"

namespace erc
{
	class JoyStickData : public Data
	{
	public:
		JoyStickData();
		~JoyStickData();
		unsigned char getType() const;
		unsigned char num_of_buttons;
		unsigned char num_of_axes;
		char *button;
		char *axis;

		friend std::istream &operator>>(std::istream &is, JoyStickData &data);
		friend std::ostream &operator<<(std::ostream &os, const JoyStickData &data);
	};
}

#endif
