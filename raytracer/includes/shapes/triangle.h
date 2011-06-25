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

#ifndef ZIYAN_RAYTRACER_SHAPES_TRIANGLE_H
#define ZIYAN_RAYTRACER_SHAPES_TRIANGLE_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <model/shape.h>

class Triangle : public Shape {
public:
	Triangle(Object *parent = 0, const material_t material = MATERIAL_DEFAULT, const point3d_t a = POINT3D_ZERO, const point3d_t b = POINT3D_ZERO, const point3d_t c = POINT3D_ZERO);
	virtual ~Triangle();
	virtual void setVertices(const point3d_t a, const point3d_t b, const point3d_t c);
	virtual void setPose(const point3d_t p = POINT3D_ZERO, const point3d_t d = POINT3D_ZERO);
	virtual point3d_t getVertexA() const;
	virtual point3d_t getVertexB() const;
	virtual point3d_t getVertexC() const;
	virtual hit_t hit(const ray_t *ray) const;
	virtual color_t trace(const ray_t *ray, const double t) const;
protected:
	point3d_t a, b, c, n;
	boost::numeric::ublas::vector<double> va, vb, vc;
};

#endif

