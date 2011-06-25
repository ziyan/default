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

#include "ball.h"

#include <math.h>
#include <GL/glut.h>

void ball(double x, double d, double a, double r, double theta) {
	double y = cos(theta + a) * d;
	double z = sin(theta + a) * d;
	glLineWidth(5);
	glLineStipple(1,0xF0F0);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex3f(-40,0,0);
		glVertex3f(x,y,z);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	glPushMatrix();
	glTranslatef(x,y,z);
	glutSolidSphere(r,50,50);
	glPopMatrix();
}
