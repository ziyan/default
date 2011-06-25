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

#ifndef CV_CAMERA_H
#define CV_CAMERA_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <cv.h>
#include <highgui.h>
#include <libplayercore/playercore.h>

class cvcamera : public Driver
{
public:
	cvcamera(ConfigFile* cf, int section);
	~cvcamera();
	int Setup();
	int Shutdown();
	int ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void *data);

private:
	void Main();

	player_devaddr_t camera_addr;
	player_camera_data_t camera_data;

	CvCapture *capture;
	int camera;
	int width, height;
	std::string show;
};

#endif

