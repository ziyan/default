#ifndef ERC_CAMERAFRAME_H
#define ERC_CAMERAFRAME_H
#include "../Data.h"
#include <iostream>

namespace erc
{
	class CameraFrame : public Data
	{
	public:
		CameraFrame();
		~CameraFrame();

		unsigned char *frame;
		unsigned short int width;
		unsigned short int height;
		unsigned short int channel;

		unsigned char getType() const;

		friend std::istream &operator>>(std::istream &is, CameraFrame &frame);
		friend std::ostream &operator<<(std::ostream &os, const CameraFrame &frame);
	};
}

#endif
