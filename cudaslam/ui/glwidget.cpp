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

#define GL_GLEXT_PROTOTYPES
#include "glwidget.h"
#include <QDebug>

#define MOUSE_MODE_NONE 0
#define MOUSE_MODE_PITCH_YAW 1
#define MOUSE_MODE_X_Y 2

GLWidget::GLWidget(World* world, Robot *robot, QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers | QGL::DoubleBuffer | QGL::Rgba), parent),
	world(world), robot(robot),
	lookat_x(0), lookat_y(0), eye_yaw(-M_PI), eye_pitch(0.7), eye_r(3.0),
	lookat_x_old(0), lookat_y_old(0), eye_yaw_old(0.0), eye_pitch_old(0.0),
	mouse_mode(MOUSE_MODE_NONE), mouse_x(0), mouse_y(0), tag_robot(robot != 0) {

	// change window title text
	this->window()->setWindowTitle("SLAM Visualization for Player");

	// tracking mouse and keyboard
	setMouseTracking(true);
	grabKeyboard();

	// build color map
	for(int i = 0; i <= 200; i++) {
		QColor c = QColor::fromHsvF((double)i / 300.0, 0.9, 1.0);
		c.getRgbF(&color_map[i][0], &color_map[i][1], &color_map[i][2]);
	}

	// build a map of laser angles
	for(int i = 0; i < 361; i++)
		laser_angles[i] = (float)(i - 180) / 360.0 * M_PI;

	// frame updater
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	timer->start(20);

	// auto buffer swap
	setAutoBufferSwap(true);
}

GLWidget::~GLWidget() {
}


/// GLWidget methods

void GLWidget::initializeGL() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	//TODO: fog is not yet working
	glEnable(GL_FOG);
	GLfloat fog_color[] = {0,0,0,1.0};
	glFogfv(GL_FOG_COLOR, fog_color);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 10);
	glFogf(GL_FOG_END, 15);
}

void GLWidget::resizeGL(int w, int h) {
	width = w;
	height = h;
	glViewport(0, 0, (GLint)w, (GLint)h);
}

void GLWidget::paintGL() {
	if(robot) {
		float x = robot->getX();
		float y = robot->getY();
		float yaw = robot->getYaw();
		static float yaw_old = yaw;

		if(tag_robot) {
			lookat_x = x;
			lookat_y = y;
			eye_yaw += yaw - yaw_old;
		}
		yaw_old = yaw;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)width/(GLfloat)height, 0.001, 400.0);
	gluLookAt(lookat_x + eye_r * cos(eye_pitch) * cos(eye_yaw),
		lookat_y + eye_r * cos(eye_pitch) * sin(eye_yaw),
		0.5 + sin(eye_pitch) * eye_r,
		lookat_x, lookat_y, 0.5, -1.0 * cos(eye_yaw), -1.0 * sin(eye_yaw), 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(robot) {
		float ranges[361];
		robot->getRanges(ranges);
		drawRobot(robot->getX(), robot->getY(), robot->getYaw(), ranges);
	}
	drawGrid();
	drawMap();
}



/// 3D content drawing functions

void GLWidget::drawRobot(float x, float y, float yaw, float ranges[361]) {
	if(eye_r < 1.5) return;
	glPushMatrix();

	glTranslatef(x, y, 0.3f);
	glRotated(yaw/M_PI * 180.0,0.0f,0.0f,1.0f);

	if(tag_robot) {
		glColor4f(1.0f,1.0f,1.0f,0.1f);
		glBegin(GL_LINES);
		glLineWidth(0.01);
		for(int i = 0; i < 361; i+= 9) {
			glVertex3f(0.0, 0.0, -0.2);
			glVertex3f(ranges[i] * cos(laser_angles[i]), ranges[i] * sin(laser_angles[i]), -0.2);
		}
		glEnd();
	}
	
	
	glScalef(0.2, 0.2, 0.3);
	glBegin(GL_QUADS);		// Draw The Cube Using quads	
	glColor3f(1.0f,1.0f,1.0f);
	glVertex3f( 1.0f, 1.0f,-1.0f);	// Top Right Of The Quad (Top)
	glVertex3f(-1.0f, 1.0f,-1.0f);	// Top Left Of The Quad (Top)
	glVertex3f(-1.0f, 1.0f, 1.0f);	// Bottom Left Of The Quad (Top)
	glVertex3f( 1.0f, 1.0f, 1.0f);	// Bottom Right Of The Quad (Top)
	glColor3f(1.0f,1.0f,1.0f);
	glVertex3f( 1.0f,-1.0f, 1.0f);	// Top Right Of The Quad (Bottom)
	glVertex3f(-1.0f,-1.0f, 1.0f);	// Top Left Of The Quad (Bottom)
	glVertex3f(-1.0f,-1.0f,-1.0f);	// Bottom Left Of The Quad (Bottom)
	glVertex3f( 1.0f,-1.0f,-1.0f);	// Bottom Right Of The Quad (Bottom)
	glColor3f(1.0f,1.0f,1.0f);
	glVertex3f( 1.0f, 1.0f, 1.0f);	// Top Right Of The Quad (Front)
	glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Left Of The Quad (Front)
	glVertex3f(-1.0f,-1.0f, 1.0f);	// Bottom Left Of The Quad (Front)
	glVertex3f( 1.0f,-1.0f, 1.0f);	// Bottom Right Of The Quad (Front)
	glColor3f(1.0f,1.0f,1.0f);
	glVertex3f( 1.0f,-1.0f,-1.0f);	// Top Right Of The Quad (Back)
	glVertex3f(-1.0f,-1.0f,-1.0f);	// Top Left Of The Quad (Back)
	glVertex3f(-1.0f, 1.0f,-1.0f);	// Bottom Left Of The Quad (Back)
	glVertex3f( 1.0f, 1.0f,-1.0f);	// Bottom Right Of The Quad (Back)
	glColor3f(1.0f,1.0f,1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Right Of The Quad (Left)
	glVertex3f(-1.0f, 1.0f,-1.0f);	// Top Left Of The Quad (Left)
	glVertex3f(-1.0f,-1.0f,-1.0f);	// Bottom Left Of The Quad (Left)
	glVertex3f(-1.0f,-1.0f, 1.0f);	// Bottom Right Of The Quad (Left)
	glColor3f(1.0f,1.0f,1.0f);
	glVertex3f( 1.0f, 1.0f,-1.0f);	// Top Right Of The Quad (Right)
	glVertex3f( 1.0f, 1.0f, 1.0f);	// Top Left Of The Quad (Right)
	glVertex3f( 1.0f,-1.0f, 1.0f);	// Bottom Left Of The Quad (Right)
	glVertex3f( 1.0f,-1.0f,-1.0f);	// Bottom Right Of The Quad (Right)
	glEnd();			// End Drawing The Cube
	glPopMatrix();
}

void GLWidget::drawGrid() {
	// grid
	#define GRID_X_MAX 100.0
	#define GRID_Y_MAX 50.0
	glBegin(GL_LINES);
	glLineWidth(0.1);
	glColor3f(0.5, 0.5, 0.5);
	for(float x = -GRID_X_MAX; x <= GRID_X_MAX; x += 1.0) {
		glVertex3f(x, -GRID_Y_MAX, -0.1);
		glVertex3f(x, GRID_Y_MAX, -0.1);
	}
	for(float y = -GRID_Y_MAX; y <= GRID_Y_MAX; y += 1.0) {
		glVertex3f(-GRID_X_MAX, y, -0.1);
		glVertex3f(GRID_X_MAX, y, -0.1);
	}
	glEnd();

}

void GLWidget::drawMap() {
	// map mesh
	Map& map = *world->getMap();
	map.sync();

	glBegin(GL_QUADS);
	
	int width = map.getWidth();
	int height = map.getHeight();
	float mpp = map.getMPP();
	int focus_x = (int)(lookat_x / mpp) + width / 2;
	int focus_y = (int)(lookat_y / mpp) + height / 2;
	int value0, value1, value2, value3;
	float dx1, dx2, dy1, dy2, dz1, dz2, dz3, dz4;
	
	for(int x = 0; x < width - 1; x++) {
		for(int y = 0; y < height - 1; y++) {
			value0 = map.getValue(x, y);
			if (value0 == MAP_UNEXPLORED) continue;
			value1 = map.getValue(x + 1, y);
			if (value1 == MAP_UNEXPLORED) continue;
			value2 = map.getValue(x + 1, y + 1);
			if (value2 == MAP_UNEXPLORED) continue;
			value3 = map.getValue(x, y + 1);
			if (value3 == MAP_UNEXPLORED) continue;

			if(tag_robot && abs(focus_x - x) + abs(focus_y - y) > 500) continue;

			dx1 = mpp * (x - width / 2);
			dx2 = mpp * (x + 1 - width / 2);
			dy1 = mpp * (y - height / 2);
			dy2 = mpp * (y + 1 - height / 2);
			dz1 = (float)value0/200.0 + 0.5;
			if(dz1 <= 0.5) dz1 = 0.0;
			dz2 = (float)value1/200.0 + 0.5;
			if(dz2 <= 0.5) dz2 = 0.0;
			dz3 = (float)value2/200.0 + 0.5;
			if(dz3 <= 0.5) dz3 = 0.0;
			dz4 = (float)value3/200.0 + 0.5;
			if(dz4 <= 0.5) dz4 = 0.0;

			
			glColor3f(color_map[value0+100][0], color_map[value0+100][1], color_map[value0+100][2]);
			glVertex3d(dx1, dy1, dz1);
			
			glColor3f(color_map[value1+100][0], color_map[value1+100][1], color_map[value1+100][2]);
			glVertex3d(dx2, dy1, dz2);
			
			glColor3f(color_map[value2+100][0], color_map[value2+100][1], color_map[value2+100][2]);
			glVertex3d(dx2, dy2, dz3);

			glColor3f(color_map[value3+100][0], color_map[value3+100][1], color_map[value3+100][2]);
			glVertex3d(dx1, dy2, dz4);
		}
		
	}

	glEnd();
}


/// The following section handles user interaction

void GLWidget::keyPressedEvent(QKeyEvent *event) {
	//TODO: for some reason this is never triggered
	qDebug("key pressed");
}

void GLWidget::keyReleasedEvent(QKeyEvent *event) {
	//TODO: for some reason this is never triggered
	qDebug("key released");
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
	if(event->button() == Qt::LeftButton)
		mouse_mode = MOUSE_MODE_X_Y;
	if(event->button() == Qt::RightButton)
		mouse_mode = MOUSE_MODE_PITCH_YAW;
	mouse_x = event->x();
	mouse_y = event->y();
	eye_pitch_old = eye_pitch;
	eye_yaw_old = eye_yaw;
	lookat_x_old = lookat_x;
	lookat_y_old = lookat_y;
}
void GLWidget::mouseMoveEvent(QMouseEvent *event) {
	if(mouse_mode == MOUSE_MODE_NONE) return;
	int dx = event->x() - mouse_x;
	int dy = event->y() - mouse_y;
	if(mouse_mode == MOUSE_MODE_X_Y) {
		float d = sqrt(((float)dx / 100.0)*((float)dx / 100.0) + ((float)dy / 100.0)*((float)dy / 100.0)) * eye_r / 5.0;
		float a = atan2(((float)dy / 100.0), ((float)dx / 100.0));
		lookat_x = lookat_x_old + sin(eye_yaw - a) * d;
		lookat_y = lookat_y_old - cos(eye_yaw - a) * d;
	}
	if(mouse_mode == MOUSE_MODE_PITCH_YAW) {
		eye_pitch = eye_pitch_old + (float)dy / 800.0;
		eye_yaw = eye_yaw_old - (float)dx / 500.0;
		if(eye_pitch < 0.0) eye_pitch = 0.0;
		if(eye_pitch > M_PI / 2.0) eye_pitch = M_PI / 2.0;
		if(eye_yaw < -M_PI) eye_yaw += M_PI + M_PI;
		if(eye_yaw > M_PI) eye_yaw -= M_PI + M_PI;
	}
}
void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
	mouse_mode = MOUSE_MODE_NONE;
}
void GLWidget::mouseDoubleClickEvent(QMouseEvent *event) {
	tag_robot = !tag_robot;
}
void GLWidget::wheelEvent(QWheelEvent *event) {
	for(int i = 0; i < abs(event->delta()); i++)
		eye_r *= (event->delta() < 0) ? 1.001 : 1.0 / 1.001;
}

