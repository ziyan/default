/*
 * Copyright (C) 2009 Ziyan Zhou <zhou@ziyan.info>
 * Visit http://ziyan.info/tag/head-tracking/ for more information
 *
 * This file is part of Head Tracking Display.
 *
 * Head Tracking Display is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Head Tracking Display is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Head Tracking Display.  If not, see <http://www.gnu.org/licenses/>.
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "camera.h"
#include <cstring>
#include <opencv/cv.h>

using namespace PlayerCc;

int camera_retrieve(CameraProxy *camera, IplImage** frame)
{
	if(!camera || !frame) return -1;

	if(camera->GetFormat() != PLAYER_CAMERA_FORMAT_RGB888) return -2;

	// check if the image requires decompression
	if (camera->GetCompression() == PLAYER_CAMERA_COMPRESS_JPEG)
		camera->Decompress();

	if (*frame == NULL)
		*frame = cvCreateImage(cvSize(camera->GetWidth(),camera->GetHeight()),IPL_DEPTH_8U,3);
	else if((**frame).width != (int)camera->GetWidth() ||
		(**frame).height != (int)camera->GetHeight() ||
		(**frame).nChannels != 3 ||
		(**frame).depth != IPL_DEPTH_8U)
		return -1; // input frame is not up to spec
	camera->GetImage((uint8_t*)(**frame).imageData);
	
	// copy data
	for (int i = 0; i<(**frame).width; i++) {
		for (int j = 0; j<(**frame).height; j++) {
			int jj = (**frame).width * j * 3 + i * 3;
			// swap color
			char temp = (**frame).imageData[jj + 0];
			(**frame).imageData[jj + 0] = (**frame).imageData[jj + 2];
			(**frame).imageData[jj + 2] = temp;
		}
	}
	return 0;
}

