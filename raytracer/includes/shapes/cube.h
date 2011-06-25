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

#ifndef ZIYAN_RAYTRACER_SHAPES_CUBE_H
#define ZIYAN_RAYTRACER_SHAPES_CUBE_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <model/shape.h>
#include <shapes/triangle.h>

class Cube : public Shape {
public:
	Cube(Object *parent = 0, const material_t material = MATERIAL_DEFAULT, const point3d_t dim = POINT3D(0.5, 0.5, 0.5), const point3d_t p = POINT3D_ZERO, const point3d_t d = POINT3D_ZERO);
	virtual ~Cube();
protected:
	point3d_t dim;
	Triangle top1, top2, bottom1, bottom2, left1, left2, right1, right2, front1, front2, back1, back2;
};

#endif

