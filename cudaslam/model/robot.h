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

#ifndef SLAM_MODEL_ROBOT_H
#define SLAM_MODEL_ROBOT_H

#include "world.h"

#include <QObject>
#include <QThread>

namespace PlayerCc {
	class PlayerClient;
	class Position2dProxy;
	class LaserProxy;
}

/**
 * Class Robot
 * Connects to player server and update map
 */
class Robot : public QThread {
	Q_OBJECT
public:
	/**
	 * Constructor for Robot
	 */
	Robot(World *world,
		const char* hostname = "localhost",
		const int port = 6665,
		const int position2d_index = 0,
		const int laser_index = 0);
	virtual ~Robot();

	/// data accessors
	virtual float getX() const;
	virtual float getY() const;
	virtual float getYaw() const;
	virtual void getRanges(float *data) const;

Q_SIGNALS:
	void laserDataReceived(const float x, const float y, const float yaw, const float *ranges);

protected Q_SLOTS:
	virtual void stop();

protected:
	virtual void run();

	World *world;
	PlayerCc::PlayerClient *robot;
	PlayerCc::Position2dProxy *position2d;
	PlayerCc::LaserProxy *laser;
	float x, y, yaw; // robot position
	float ranges[361]; // last sensor reading
	bool running; // flag to indicate when to quit thread
};

#endif //SLAM_MODEL_ROBOT_H

