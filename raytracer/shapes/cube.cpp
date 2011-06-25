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

#include <shapes/cube.h>

Cube::Cube(Object *parent, const material_t material, const point3d_t dim, const point3d_t p, const point3d_t d) :
	Shape(parent, material, p, d), dim(dim),
	top1(this, MATERIAL(COLOR_RED),
	      POINT3D(dim.x/2.0, dim.y/2.0, dim.z/2.0), // (x, y, z)
	      POINT3D(dim.x/2.0, dim.y/2.0, -dim.z/2.0), // (x, y, -z)
	      POINT3D(-dim.x/2.0, dim.y/2.0, dim.z/2.0)), // (-x, y, z)
	top2(this, MATERIAL(COLOR_RED),
	      POINT3D(-dim.x/2.0, dim.y/2.0, dim.z/2.0), // (-x, y, z)
	      POINT3D(dim.x/2.0, dim.y/2.0, -dim.z/2.0), // (x, y, -z)
	      POINT3D(-dim.x/2.0, dim.y/2.0, -dim.z/2.0)), // (-x, y, -z)
	bottom1(this, MATERIAL(COLOR_RED),
	      POINT3D(-dim.x/2.0, -dim.y/2.0, dim.z/2.0), // (-x, -y, z)
	      POINT3D(dim.x/2.0, -dim.y/2.0, -dim.z/2.0), // (x, -y, -z)
	      POINT3D(dim.x/2.0, -dim.y/2.0, dim.z/2.0)), // (x, -y, z)
	bottom2(this, MATERIAL(COLOR_RED),
	      POINT3D(-dim.x/2.0, -dim.y/2.0, -dim.z/2.0), // (-x, -y, -z)
	      POINT3D(dim.x/2.0, -dim.y/2.0, -dim.z/2.0), // (x, -y, -z)
	      POINT3D(-dim.x/2.0, -dim.y/2.0, dim.z/2.0)), // (-x. -y. z)
	left1(this, MATERIAL(COLOR_BLUE),
	      POINT3D(-dim.x/2.0, dim.y/2.0, dim.z/2.0), // (-x, y, z)
	      POINT3D(-dim.x/2.0, dim.y/2.0, -dim.z/2.0), // (-x, y, -z)
	      POINT3D(-dim.x/2.0, -dim.y/2.0, dim.z/2.0)), // (-x, -y, z)
	left2(this, MATERIAL(COLOR_BLUE),
	      POINT3D(-dim.x/2.0, dim.y/2.0, -dim.z/2.0), // (-x, y, -z)
	      POINT3D(-dim.x/2.0, -dim.y/2.0, -dim.z/2.0), // (-x, -y, -z)
	      POINT3D(-dim.x/2.0, -dim.y/2.0, dim.z/2.0)), // (-x, -y, z)
	right1(this, MATERIAL(COLOR_BLUE),
	      POINT3D(dim.x/2.0, -dim.y/2.0, dim.z/2.0), // (x, -y, z)
	      POINT3D(dim.x/2.0, -dim.y/2.0, -dim.z/2.0), // (x, -y, -z)
	      POINT3D(dim.x/2.0, dim.y/2.0, -dim.z/2.0)), // (x, y, -z)
	right2(this, MATERIAL(COLOR_BLUE),
	      POINT3D(dim.x/2.0, -dim.y/2.0, dim.z/2.0), // (x, -y, z)
	      POINT3D(dim.x/2.0, dim.y/2.0, -dim.z/2.0), // (x, y, -z)
	      POINT3D(dim.x/2.0, dim.y/2.0, dim.z/2.0)), // (x, y, z)
	front1(this, MATERIAL(COLOR_GREEN),
	      POINT3D(dim.x/2.0, dim.y/2.0, dim.z/2.0), // (x, y, z)
	      POINT3D(-dim.x/2.0, -dim.y/2.0, dim.z/2.0), // (-x, -y ,z)
	      POINT3D(dim.x/2.0, -dim.y/2.0, dim.z/2.0)), // (x, -y, z)
	front2(this, MATERIAL(COLOR_GREEN),
	      POINT3D(dim.x/2.0, dim.y/2.0, dim.z/2.0), // (x, y, z)
	      POINT3D(-dim.x/2.0, dim.y/2.0, dim.z/2.0), // (-x, y, z)
	      POINT3D(-dim.x/2.0, -dim.y/2.0, dim.z/2.0)), // (-x, -y, z)
	back1(this, MATERIAL(COLOR_GREEN),
	      POINT3D(dim.x/2.0, -dim.y/2.0, -dim.z/2.0), // (x, -y, -z)
	      POINT3D(-dim.x/2.0, -dim.y/2.0, -dim.z/2.0), // (-x, -y, -z)
	      POINT3D(dim.x/2.0, dim.y/2.0, -dim.z/2.0)), // (x, y, -z)
	back2(this, MATERIAL(COLOR_GREEN),
	      POINT3D(-dim.x/2.0, -dim.y/2.0, -dim.z/2.0), // (-x, -y, -z)
	      POINT3D(-dim.x/2.0, dim.y/2.0, -dim.z/2.0), // (-x, y, -z)
	      POINT3D(dim.x/2.0, dim.y/2.0, -dim.z/2.0)) // (x, y, -z)
{
}

Cube::~Cube() {
}
