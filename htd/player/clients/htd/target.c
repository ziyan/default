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

#include "target.h"

#include <math.h>
#include <GL/glut.h>
#define FINENESS (M_PI / 180.0)

void target(double x, double y, double z, double r, texture_t *texture) {
	glLineWidth(5);
	glLineStipple(1,0xFF00);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex3f(-40,0,0);
		glVertex3f(x,y,z);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	glPushMatrix();
	glTranslatef(x,y,z);
	glEnable(GL_TEXTURE_2D);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture->width, texture->height, GL_RGB, GL_UNSIGNED_BYTE, texture->image);

	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5, 0.5);
	glVertex3f(0.0, 0.0, 0.0);
	double a;
	for(a = -M_PI; a < M_PI; a += FINENESS) {
		glTexCoord2f(cos(a)/2.0+0.5, sin(a)/2.0+0.5);
		glVertex3f(0.0, cos(a)*r, sin(a)*r);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
