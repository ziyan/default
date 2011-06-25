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

#include "grid.h"

#include <math.h>
#include <GL/glut.h>

void grid(double width, double height, double depth, double space) {
	glLineWidth(2);
	double x,y,z;
	for(x=-width/2;x<=width/2;x+=space)
	{
		glBegin(GL_LINES);
			glVertex3f(0.0,x,-height/2);
			glVertex3f(-depth/2,x,-height/2);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(0.0,x,height/2);
			glVertex3f(-depth/2,x,height/2);
		glEnd();
	}
	for(y=-height/2;y<=height/2;y+=space)
	{
		glBegin(GL_LINES);
			glVertex3f(0.0,-width/2,y);
			glVertex3f(-depth/2,-width/2,y);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(0.0,width/2,y);
			glVertex3f(-depth/2,width/2,y);
		glEnd();
	}
	for(z=-depth/2;z<=0.0;z+=space)
	{
		glBegin(GL_LINES);
			glVertex3f(z,-width/2,height/2);
			glVertex3f(z,width/2,height/2);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(z,-width/2,-height/2);
			glVertex3f(z,width/2,-height/2);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(z,width/2,-height/2);
			glVertex3f(z,width/2,height/2);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(z,-width/2,-height/2);
			glVertex3f(z,-width/2,height/2);
		glEnd();
	}
}

