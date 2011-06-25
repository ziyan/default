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

#include "screen.h"

#include <math.h>
#include <GL/glut.h>

// screen mash
void screen(double hangle, double vangle, double radius, int segment, texture_t *texture) {
	glEnable(GL_TEXTURE_2D);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture->width, texture->height, GL_RGB, GL_UNSIGNED_BYTE, texture->image);
	int v,h;
	double ha1, ha2, va1, va2, hc1, hc2, vc1, vc2;
	glBegin(GL_QUADS);
	for(v=-segment/2;v<segment/2;v++) {
		va1 = vangle/(double)segment*v;
		va2 = vangle/(double)segment*(v+1);
		vc1 = -(double)v/(double)segment - 0.5;
		vc2 = -(double)(v+1)/(double)segment - 0.5;
		for(h=-segment/2;h<segment/2;h++) {
			ha1 = hangle/(double)segment*h;
			ha2 = hangle/(double)segment*(h+1);
			hc1 = -(double)h/(double)segment - 0.5;
			hc2 = -(double)(h+1)/(double)segment - 0.5;

			glTexCoord2f(hc1, vc1);
			glVertex3f(-radius*cos(va1)*cos(ha1), radius*cos(va1)*sin(ha1), radius*sin(va1));
			glTexCoord2f(hc2, vc1);
			glVertex3f(-radius*cos(va1)*cos(ha2), radius*cos(va1)*sin(ha2), radius*sin(va1));
			glTexCoord2f(hc2, vc2);
			glVertex3f(-radius*cos(va2)*cos(ha2), radius*cos(va2)*sin(ha2), radius*sin(va2));
			glTexCoord2f(hc1, vc2);
			glVertex3f(-radius*cos(va2)*cos(ha1), radius*cos(va2)*sin(ha1), radius*sin(va2));
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

