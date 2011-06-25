/*
 * Copyright (C) 2009 Ziyan Zhou <zhou@ziyan.info>
 * Visit http://ziyan.info/tag/ritcg2/ for more information
 *
 * This file is part of Raytracer.
 *
 * Raytracer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Raytracer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Raytracer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <shapes/cone.h>

#include <iostream>
#include <cmath>

Cone::Cone(Object *parent, const material_t material, const double height, const int height_tess, const double radius, const int radius_tess, const point3d_t p, const point3d_t d) :
	Shape(parent, material, p, d), height(height), radius(radius) {
	double ratio = atan(radius/height);
	double l = sqrt(height*height+radius*radius);
	double angle = M_PI / (double)radius_tess;
	for(int r = 0; r < radius_tess; r++) {
		double a = (double)r * M_PI * 2.0 / (double)radius_tess;
		// construct the bottom
		bottom.push_back(new Triangle(this, MATERIAL(COLOR_RED),
			POINT3D(0.0, 0.0, 0.0),
			POINT3D(radius*cos(a-angle), 0.0, radius*sin(a-angle)),
			POINT3D(radius*cos(a+angle), 0.0, radius*sin(a+angle))));

		// construct the sides
		for(int h = 0; h < height_tess; h++) {
			double l1 = (double)h * l / (double)height_tess;
			double l2 = (double)(h+1) * l / (double)height_tess;
			double r1 = sin(ratio) * l1;
			double r2 = sin(ratio) * l2;
			double y1 = height - cos(ratio) * l1;
			double y2 = height - cos(ratio) * l2;
			sides.push_back(new Triangle(this, MATERIAL(COLOR_YELLOW),
				POINT3D(r1*cos(a-angle), y1, r1*sin(a-angle)),
				POINT3D(r1*cos(a+angle), y1, r1*sin(a+angle)),
				POINT3D(r2*cos(a-angle), y2, r2*sin(a-angle))));
			sides.push_back(new Triangle(this, MATERIAL(COLOR_BLUE),
				POINT3D(r1*cos(a+angle), y1, r1*sin(a+angle)),
				POINT3D(r2*cos(a+angle), y2, r2*sin(a+angle)),
				POINT3D(r2*cos(a-angle), y2, r2*sin(a-angle))));
		}
	}
}

Cone::~Cone() {
}
