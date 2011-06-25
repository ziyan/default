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

#ifndef ZIYAN_RAYTRACER_SHAPES_SPHERE_H
#define ZIYAN_RAYTRACER_SHAPES_SPHERE_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <model/shape.h>

class Sphere : public Shape {
public:
	Sphere(Object *parent = 0, const material_t material = MATERIAL_DEFAULT, const double radius = 0.0, const point3d_t p = POINT3D_ZERO);
	virtual ~Sphere();
	virtual void setRadius(const double radius);
	virtual double getRadius() const;
	virtual point3d_t getCenter() const;
	virtual void setPose(const point3d_t p, const point3d_t d = POINT3D_ZERO);
	virtual hit_t hit(const ray_t *ray) const;
	virtual color_t trace(const ray_t *ray, const double t) const;
protected:
	double radius;
	point3d_t c;
};

#endif

