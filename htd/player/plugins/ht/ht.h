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

#ifndef HT_H
#define HT_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <libplayercore/playercore.h>

class ht : public Driver
{
public:
	ht(ConfigFile* cf, int section);
	~ht();
	int Setup();
	int Shutdown();
	int ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void *data);

private:
	void Main();

	player_devaddr_t p3d_addr;
	player_devaddr_t top_left_addr, top_right_addr, side_top_addr, side_bottom_addr;
	Device *top_left_dev, *top_right_dev, *side_top_dev, *side_bottom_dev;

	player_pose3d_t top_left_pose, top_right_pose, side_top_pose, side_bottom_pose;
	bool top_left_blob, top_right_blob, side_top_blob, side_bottom_blob;
	double top_left_x, top_left_y, top_right_x, top_right_y, side_top_z, side_bottom_z;
	double top_left_hfov, top_left_vfov, top_right_hfov, top_right_vfov, side_top_hfov, side_top_vfov, side_bottom_hfov, side_bottom_vfov;
	double x_weight, y_weight, z_weight;
	player_position3d_data_t p3d;
};

#endif

