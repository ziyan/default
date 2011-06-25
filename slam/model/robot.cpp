/*
 * Copyright (C) 2009 Ziyan Zhou <zhou@ziyan.info>
 * Visit http://ziyan.info/tag/ritcg2slam/ for more information
 *
 * This file is part of SLAM Visualization for Player.
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "robot.h"
#include <QApplication>
#include <libplayerc++/playerc++.h>

Robot::Robot(World *world,
	const char *hostname,
	const int port,
	const int camera_index,
	const int position2d_index,
	const int laser_index) :
	QThread(world),
	world(world),
	robot(new PlayerCc::PlayerClient(hostname, port)),
	position2d(new PlayerCc::Position2dProxy(robot, position2d_index)),
	laser(new PlayerCc::LaserProxy(robot, laser_index)),
	camera(camera_index >= 0 ? new PlayerCc::CameraProxy(robot, camera_index) : 0),
	running(true) {
	connect(QApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(stop()));
	this->start();
}

Robot::~Robot() {	
	delete position2d;
	delete laser;
	if(camera) delete camera;
	delete robot;
}


void Robot::stop() {
	running = false;
}

void Robot::run() {
	uint8_t *frame = 0;
	while(running) {
		robot->Read();
		if(!running) break;
		if(laser->GetCount() != 361) continue;		
		for(int i=0;i<361;i++)
			ranges[i] = laser->GetRange(i);

		// location information
		x = position2d->GetXPos();
		y = position2d->GetYPos();
		yaw = position2d->GetYaw();

		// map laser reading		
		world->getMap()->mapLaser(x, y, yaw, ranges);

		// map camera frame		
		if(!camera || !camera->IsValid()) continue;
		int width = camera->GetWidth();
		int height = camera->GetHeight();
		if(width == 0 || height == 0) continue;
		if(camera->GetFormat() != PLAYER_CAMERA_FORMAT_RGB888) continue;
		if(camera->GetCompression() == PLAYER_CAMERA_COMPRESS_JPEG) camera->Decompress();
		if(!frame) frame = new uint8_t[width * height * 3];
		camera->GetImage(frame);
		//TODO: this camera geometry setting has been hardcoded
		world->getMap()->mapFrame(x, y, 1.0, -M_PI / 180.0 * 30.0, yaw, M_PI / 180.0 * 65.0, M_PI / 180.0 * 45.0, width, height, frame);
	}
	if(frame) delete [] frame;
}

double Robot::getX() const {
	return x;
}

double Robot::getY() const {
	return y;
}

double Robot::getYaw() const {
	return yaw;
}

void Robot::getRanges(double *data) const {
	for(int i = 0; i < 361; i++)
		data[i] = ranges[i];
}

