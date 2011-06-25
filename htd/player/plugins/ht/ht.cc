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

#include "ht.h"
#include <cmath>
#include <unistd.h>

using namespace std;

ht::ht(ConfigFile* cf, int section) : Driver(cf, section) {
	top_left_pose.px = cf->ReadTupleFloat(section, "top_left_pose", 0, -1.905);
	top_left_pose.py = cf->ReadTupleFloat(section, "top_left_pose", 1, 0.03);
	top_left_pose.pz = cf->ReadTupleFloat(section, "top_left_pose", 2, 4.1656);
	top_left_pose.proll = cf->ReadTupleFloat(section, "top_left_pose", 3, 0.0) * M_PI / 180.0;
	top_left_pose.ppitch = cf->ReadTupleFloat(section, "top_left_pose", 4, -43.45) * M_PI / 180.0;
	top_left_pose.pyaw = cf->ReadTupleFloat(section, "top_left_pose", 5, 0.0) * M_PI / 180.0;
	PLAYER_MSG6(8,"ht: config: top_left_pose = (%f, %f, %f, %f, %f, %f)", top_left_pose.px, top_left_pose.py, top_left_pose.pz, top_left_pose.proll, top_left_pose.ppitch, top_left_pose.pyaw);

	top_left_hfov = cf->ReadTupleFloat(section, "top_left_fov", 0, 43.4010735) * M_PI / 180.0 / 2.0;
	top_left_vfov = cf->ReadTupleFloat(section, "top_left_fov", 1, 34.5434852) * M_PI / 180.0 / 2.0;
	PLAYER_MSG2(8,"ht: config: top_left_fov = (%f, %f)", top_left_hfov / M_PI * 180.0, top_left_vfov / M_PI * 180.0);

	top_right_pose.px = cf->ReadTupleFloat(section, "top_right_pose", 0, -1.905);
	top_right_pose.py = cf->ReadTupleFloat(section, "top_right_pose", 1, -0.03);
	top_right_pose.pz = cf->ReadTupleFloat(section, "top_right_pose", 2, 4.1656);
	top_right_pose.proll = cf->ReadTupleFloat(section, "top_right_pose", 3, 0.0) * M_PI / 180.0;
	top_right_pose.ppitch = cf->ReadTupleFloat(section, "top_right_pose", 4, -43.45) * M_PI / 180.0;
	top_right_pose.pyaw = cf->ReadTupleFloat(section, "top_right_pose", 5, 0.0) * M_PI / 180.0;
	PLAYER_MSG6(8,"ht: config: top_right_pose = (%f, %f, %f, %f, %f, %f)", top_right_pose.px, top_right_pose.py, top_right_pose.pz, top_right_pose.proll, top_right_pose.ppitch, top_right_pose.pyaw);

	top_right_hfov = cf->ReadTupleFloat(section, "top_right_fov", 0, 43.4010735) * M_PI / 180.0 / 2.0;
	top_right_vfov = cf->ReadTupleFloat(section, "top_right_fov", 1, 34.5434852) * M_PI / 180.0 / 2.0;
	PLAYER_MSG2(8,"ht: config: top_right_fov = (%f, %f)", top_right_hfov / M_PI * 180.0, top_right_vfov / M_PI * 180.0);

	side_top_pose.px = cf->ReadTupleFloat(section, "side_top_pose", 0, -0.4572);
	side_top_pose.py = cf->ReadTupleFloat(section, "side_top_pose", 1, -2.1844);
	side_top_pose.pz = cf->ReadTupleFloat(section, "side_top_pose", 2, 0.1651);
	side_top_pose.proll = cf->ReadTupleFloat(section, "side_top_pose", 3, 0.0) * M_PI / 180.0;
	side_top_pose.ppitch = cf->ReadTupleFloat(section, "side_top_pose", 4, 0.0) * M_PI / 180.0;
	side_top_pose.pyaw = cf->ReadTupleFloat(section, "side_top_pose", 5, 60.0) * M_PI / 180.0;
	PLAYER_MSG6(8,"ht: config: side_top_pose = (%f, %f, %f, %f, %f, %f)", side_top_pose.px, side_top_pose.py, side_top_pose.pz, side_top_pose.proll, side_top_pose.ppitch, side_top_pose.pyaw);

	side_top_hfov = cf->ReadTupleFloat(section, "side_top_fov", 0, 43.4010735) * M_PI / 180.0 / 2.0;
	side_top_vfov = cf->ReadTupleFloat(section, "side_top_fov", 1, 34.5434852) * M_PI / 180.0 / 2.0;
	PLAYER_MSG2(8,"ht: config: side_top_fov = (%f, %f)", side_top_hfov / M_PI * 180.0, side_top_vfov / M_PI * 180.0);

	side_bottom_pose.px = cf->ReadTupleFloat(section, "side_bottom_pose", 0, -0.4572);
	side_bottom_pose.py = cf->ReadTupleFloat(section, "side_bottom_pose", 1, -2.1844);
	side_bottom_pose.pz = cf->ReadTupleFloat(section, "side_bottom_pose", 2, 0.1151);
	side_bottom_pose.proll = cf->ReadTupleFloat(section, "side_bottom_pose", 3, 0.0) * M_PI / 180.0;
	side_bottom_pose.ppitch = cf->ReadTupleFloat(section, "side_bottom_pose", 4, 0.0) * M_PI / 180.0;
	side_bottom_pose.pyaw = cf->ReadTupleFloat(section, "side_bottom_pose", 5, 60.0) * M_PI / 180.0;
	PLAYER_MSG6(8,"ht: config: side_bottom_pose = (%f, %f, %f, %f, %f, %f)", side_bottom_pose.px, side_bottom_pose.py, side_bottom_pose.pz, side_bottom_pose.proll, side_bottom_pose.ppitch, side_bottom_pose.pyaw);

	side_bottom_hfov = cf->ReadTupleFloat(section, "side_bottom_fov", 0, 43.4010735) * M_PI / 180.0 / 2.0;
	side_bottom_vfov = cf->ReadTupleFloat(section, "side_bottom_fov", 1, 34.5434852) * M_PI / 180.0 / 2.0;
	PLAYER_MSG2(8,"ht: config: side_bottom_fov = (%f, %f)", side_bottom_hfov / M_PI * 180.0, side_bottom_vfov / M_PI * 180.0);

	x_weight = cf->ReadTupleFloat(section, "weight", 0, 0.4);
	y_weight = cf->ReadTupleFloat(section, "weight", 1, 0.4);
	z_weight = cf->ReadTupleFloat(section, "weight", 2, 0.025);

	if (cf->ReadDeviceAddr(&p3d_addr, section,  "provides", PLAYER_POSITION3D_CODE, -1, NULL) != 0) {
		PLAYER_ERROR("ht: cannot find p3d addr in provides setting");
		this->SetError(-1);
		return;
	}
	if (this->AddInterface(p3d_addr)) {
		PLAYER_ERROR("ht: cannot add p3d interface");
		this->SetError(-1);
		return;
	}
	if (cf->ReadDeviceAddr(&top_left_addr, section, "requires", PLAYER_BLOBFINDER_CODE, -1, "top_left") != 0) {
		PLAYER_ERROR("ht: cannot find top left blobfinder addr");
		this->SetError(-1);
		return;
	}
	if (cf->ReadDeviceAddr(&top_right_addr, section, "requires", PLAYER_BLOBFINDER_CODE, -1, "top_right") != 0) {
		PLAYER_ERROR("ht: cannot find top right blobfinder addr");
		this->SetError(-1);
		return;
	}
	if (cf->ReadDeviceAddr(&side_top_addr, section, "requires", PLAYER_BLOBFINDER_CODE, -1, "side_top") != 0) {
		PLAYER_ERROR("ht: cannot find side top blobfinder addr");
		this->SetError(-1);
		return;
	}
	if (cf->ReadDeviceAddr(&side_bottom_addr, section, "requires", PLAYER_BLOBFINDER_CODE, -1, "side_bottom") != 0) {
		PLAYER_ERROR("ht: cannot find side bottom blobfinder addr");
		this->SetError(-1);
		return;
	}
}

ht::~ht() {
}

int ht::Setup() {
	top_left_blob = false;
	top_right_blob = false;
	side_top_blob = false;
	side_bottom_blob = false;
	p3d.pos.px = 0.0;
	p3d.pos.py = 0.0;
	p3d.pos.pz = 0.0;
	if(!(top_left_dev = deviceTable->GetDevice(top_left_addr))) {
		PLAYER_ERROR("ht: unable to locate suitable top left blobfinder device");
		return -1;
	}
	if(top_left_dev->Subscribe(InQueue) != 0) {
		PLAYER_ERROR("ht: unable to subscribe to top left blobfinder device");
		return -1;
	}
	if(!(top_right_dev = deviceTable->GetDevice(top_right_addr))) {
		PLAYER_ERROR("ht: unable to locate suitable top right blobfinder device");
		return -1;
	}
	if(top_right_dev->Subscribe(InQueue) != 0) {
		PLAYER_ERROR("ht: unable to subscribe to top right blobfinder device");
		return -1;
	}
	if(!(side_top_dev = deviceTable->GetDevice(side_top_addr))) {
		PLAYER_ERROR("ht: unable to locate suitable side top blobfinder device");
		return -1;
	}
	if(side_top_dev->Subscribe(InQueue) != 0) {
		PLAYER_ERROR("ht: unable to subscribe to side top blobfinder device");
		return -1;
	}
	if(!(side_bottom_dev = deviceTable->GetDevice(side_bottom_addr))) {
		PLAYER_ERROR("ht: unable to locate suitable side bottom blobfinder device");
		return -1;
	}
	if(side_bottom_dev->Subscribe(InQueue) != 0) {
		PLAYER_ERROR("ht: unable to subscribe to side bottom blobfinder device");
		return -1;
	}
	PLAYER_MSG0(8,"ht: setup");
	this->StartThread();
	return 0;
}

int ht::Shutdown() {
	PLAYER_MSG0(8,"ht: shutting down");
	this->StopThread();
	top_left_dev->Unsubscribe(this->InQueue);
	top_right_dev->Unsubscribe(this->InQueue);
	side_top_dev->Unsubscribe(this->InQueue);
	side_bottom_dev->Unsubscribe(this->InQueue);
	PLAYER_MSG0(8,"ht: shutted down");
	return 0;
}

int ht::ProcessMessage(QueuePointer &resp_queue, player_msghdr *hdr, void *data) {
	if(Message::MatchMessage(hdr, PLAYER_MSGTYPE_DATA, PLAYER_BLOBFINDER_DATA_BLOBS, top_left_addr)) {
		top_left_blob = false;
		player_blobfinder_data_t* d = (player_blobfinder_data_t*)data;
		if(d->blobs_count <= 0) return 0;
		top_left_blob = true;
		double v = -(double)((int)d->blobs[0].bottom - (int)d->height/2) / (double)(d->height/2) * top_left_vfov;
		double h = -(double)((int)d->blobs[0].x - (int)d->width/2) / (double)(d->width/2) * top_left_hfov;
		top_left_x = -top_left_pose.pz / tan(top_left_pose.ppitch + v);
		top_left_y = sqrt(top_left_pose.pz*top_left_pose.pz + top_left_x*top_left_x) * tan(top_left_pose.pyaw + h);
		top_left_x += top_left_pose.px;
		top_left_y += top_left_pose.py;
		PLAYER_MSG2(9,"ht: top left guessing (%f, %f, 0.0)", top_left_x, top_left_y);			
		return 0;
	}
	if(Message::MatchMessage(hdr, PLAYER_MSGTYPE_DATA, PLAYER_BLOBFINDER_DATA_BLOBS, top_right_addr)) {
		top_right_blob = false;
		player_blobfinder_data_t* d = (player_blobfinder_data_t*)data;
		if(d->blobs_count <= 0) return 0;
		top_right_blob = true;
		double v = -(double)((int)d->blobs[0].bottom - (int)d->height/2) / (double)(d->height/2) * top_right_vfov;
		double h = -(double)((int)d->blobs[0].x - (int)d->width/2) / (double)(d->width/2) * top_right_hfov;
		top_right_x = -top_right_pose.pz / tan(top_right_pose.ppitch + v);
		top_right_y = sqrt(top_right_pose.pz*top_right_pose.pz + top_right_x*top_right_x) * tan(top_right_pose.pyaw + h);
		top_right_x += top_right_pose.px;
		top_right_y += top_right_pose.py;
		PLAYER_MSG2(9,"ht: top right guessing (%f, %f, 0.0)", top_right_x, top_right_y);
		return 0;
	}
	if(Message::MatchMessage(hdr, PLAYER_MSGTYPE_DATA, PLAYER_BLOBFINDER_DATA_BLOBS, side_top_addr)) {
		side_top_blob = false;
		player_blobfinder_data_t* d = (player_blobfinder_data_t*)data;
		if(d->blobs_count <= 0) return 0;
		double yaw = atan2(p3d.pos.py - side_top_pose.py, p3d.pos.px - side_top_pose.px);
		double h = -(double)((int)d->blobs[0].x - (int)d->width/2) / (double)(d->width/2) * side_top_hfov;
		//TODO: hardcode
		if(fabs(h - (yaw - side_top_pose.pyaw)) / (2.0 * side_top_hfov) > 0.25) { // deviate less than 15%
			PLAYER_MSG2(9,"ht: side top yaw should be %f, really is %f, ignoring", (yaw - side_top_pose.pyaw), h);
			return 0;
		}
		side_top_blob = true;
		double v = -(double)((int)d->blobs[0].top - (int)d->height/2) / (double)(d->height/2) * side_top_vfov;
		double l = sqrt((p3d.pos.px-side_top_pose.px)*(p3d.pos.px-side_top_pose.px) + (p3d.pos.py-side_top_pose.py)*(p3d.pos.py-side_top_pose.py));
		side_top_z = tan(side_top_pose.ppitch + v) * l;
		side_top_z += side_top_pose.pz;
		PLAYER_MSG1(9,"ht: side top guessing (0.0, 0.0, %f)", side_top_z);
		return 0;
	}
	if(Message::MatchMessage(hdr, PLAYER_MSGTYPE_DATA, PLAYER_BLOBFINDER_DATA_BLOBS, side_bottom_addr)) {
		side_bottom_blob = false;
		player_blobfinder_data_t* d = (player_blobfinder_data_t*)data;
		if(d->blobs_count <= 0) return 0;
		double yaw = atan2(p3d.pos.py - side_bottom_pose.py, p3d.pos.px - side_bottom_pose.px);
		double h = -(double)((int)d->blobs[0].x - (int)d->width/2) / (double)(d->width/2) * side_bottom_hfov;
		//TODO: hardcode
		if(fabs(h - (yaw - side_bottom_pose.pyaw)) / (2.0 * side_bottom_hfov) > 0.25) { // deviate less than 15%
			PLAYER_MSG2(9,"ht: side bottom yaw should be %f, really is %f, ignoring", (yaw - side_top_pose.pyaw), h);
			return 0;
		}
		side_bottom_blob = true;
		double v = -(double)((int)d->blobs[0].top - (int)d->height/2) / (double)(d->height/2) * side_bottom_vfov;
		double l = sqrt((p3d.pos.px-side_bottom_pose.px)*(p3d.pos.px-side_bottom_pose.px) + (p3d.pos.py-side_bottom_pose.py)*(p3d.pos.py-side_bottom_pose.py));
		side_bottom_z = tan(side_bottom_pose.ppitch + v) * l;
		side_bottom_z += side_bottom_pose.pz;
		PLAYER_MSG1(9,"ht: side bottom guessing (0.0, 0.0, %f)", side_bottom_z);
		return 0;
	}
	return -1;
}

void ht::Main() {
	PLAYER_MSG0(8,"ht: thread started");
	double x = 0.0, y = 0.0, z = 0.0;
	for(;;) {
		pthread_testcancel();
		this->ProcessMessages();
		x = 0.0, y = 0.0, z = 0.0;
		if(top_left_blob) {
			x += top_left_x;
			y += top_left_y;
		}
		if(top_right_blob) {
			x += top_right_x;
			y += top_right_y;
		}
		if(top_left_blob && top_right_blob) {
			x /= 2.0;
			y /= 2.0;
		}
		if(top_left_blob || top_right_blob) {
			if(fabs(p3d.pos.px - x) < 0.75) //TODO: hardcode
				p3d.pos.px = p3d.pos.px * (1.0 - 0.05) + 0.05 * x;
			else
				p3d.pos.px = p3d.pos.px * (1.0 - x_weight) + x_weight * x;
			if(fabs(p3d.pos.py - y) < 0.75) //TODO: hardcode
				p3d.pos.py = p3d.pos.py * (1.0 - 0.05) + 0.05 * y;
			else
				p3d.pos.py = p3d.pos.py * (1.0 - y_weight) + y_weight * y;
		}
		if(side_top_blob) {
			z += side_top_z;
		}
		if(side_bottom_blob) {
			z += side_bottom_z;
		}
		if(side_top_blob && side_bottom_blob) {
			z /= 2.0;
		}
		z *= 2.0;
		if(side_top_blob || side_bottom_blob) {
			p3d.pos.pz = p3d.pos.pz * (1.0 - z_weight) + z_weight * z;
		}
		this->Publish(p3d_addr, PLAYER_MSGTYPE_DATA, PLAYER_POSITION3D_DATA_STATE, &p3d);
		//PLAYER_MSG3(8,"ht: position (%f, %f, %f)", p3d.pos.px, p3d.pos.py, p3d.pos.pz);
		//top_left_blob = top_right_blob = side_top_blob = false;
		usleep(25000);
	}
}

Driver* ht_Init(ConfigFile *cf, int section) {
	PLAYER_MSG0(8,"ht: initializing");
	return new ht(cf, section);
}

void ht_Register(DriverTable* table) {
	table->AddDriver("ht", ht_Init);
}

extern "C" {
	int player_driver_init(DriverTable *table) {
		ht_Register(table);
		return 0;
	}
}


