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
	const int position2d_index,
	const int laser_index) :
	QThread(world),
	world(world),
	robot(new PlayerCc::PlayerClient(hostname, port)),
	position2d(new PlayerCc::Position2dProxy(robot, position2d_index)),
	laser(new PlayerCc::LaserProxy(robot, laser_index)),
	running(true) {
	connect(QApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(stop()));
	connect(this, SIGNAL(laserDataReceived(const float, const float, const float, const float*)), world->getMap(), SLOT(mapLaser(const float, const float, const float, const float*)));
	this->start();
}

Robot::~Robot() {
	delete position2d;
	delete laser;
	delete robot;
}


void Robot::stop() {
	running = false;
}

void Robot::run() {
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
		Q_EMIT laserDataReceived(x, y, yaw, ranges);
	}
}

float Robot::getX() const {
	return x;
}

float Robot::getY() const {
	return y;
}

float Robot::getYaw() const {
	return yaw;
}

void Robot::getRanges(float *data) const {
	for(int i = 0; i < 361; i++)
		data[i] = ranges[i];
}

