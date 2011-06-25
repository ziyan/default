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

#ifndef SLAM_UI_GLWIDGET_H
#define SLAM_UI_GLWIDGET_H

#include "../model/world.h"
#include "../model/robot.h"

#include <QtOpenGL>

/**
 * Class GLWidget
 * Main 3D world display
 */
class GLWidget : public QGLWidget {
	Q_OBJECT
public:
	GLWidget(World *world, Robot *robot = 0, QWidget *parent = 0);
	virtual ~GLWidget();

protected:
	/// GLWidget methods
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

	/// 3D content drawing
	virtual void drawRobot(double x, double y, double yaw, double ranges[361]);
	virtual void drawMap();
	virtual void drawGrid();

	/// user interactions
	virtual void keyPressedEvent(QKeyEvent *event);
	virtual void keyReleasedEvent(QKeyEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseDoubleClickEvent(QMouseEvent *event);
	virtual void wheelEvent(QWheelEvent *event);


	double color_map[201][3]; // cached color map
	double laser_angles[361]; // cached map of laser angles

	World *world;
	Robot *robot;
	int width, height; // GLWidget viewport size

	// user interaction related
	double lookat_x, lookat_y, eye_yaw, eye_pitch, eye_r; // viewing angle
	double lookat_x_old, lookat_y_old, eye_yaw_old, eye_pitch_old; // copy of viewing angle
	int mouse_mode, mouse_x, mouse_y; // mouse state
	bool tag_robot; // wether to following robot or not
};

#endif //SLAM_UI_GLWIDGET_H

