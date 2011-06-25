#ifndef ERC_CAMERADATA_H
#define ERC_CAMERADATA_H

#define CAMERA_NO_MIRROR 0
#define CAMERA_LEFT_MIRROR 1
#define CAMERA_RIGHT_MIRROR 2
#define CAMERA_NO_LINES 3

#include "../Data.h"
#include "../../common/Line.h"
#include <iostream>

namespace erc
{
	class CameraData : public Data
	{
	public:
		unsigned short int width;
		unsigned short int height;
		Line left;
		Line right;
		Line center;
		unsigned char state;

		unsigned char getType() const;

                friend std::istream &operator>>(std::istream &is, CameraData &data);
                friend std::ostream &operator<<(std::ostream &os, const CameraData &data);
	};
}

#endif

