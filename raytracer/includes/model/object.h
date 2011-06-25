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

#ifndef ZIYAN_RAYTRACER_MODEL_OBJECT_H
#define ZIYAN_RAYTRACER_MODEL_OBJECT_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <common/point3d.h>
#include <common/ray.h>
#include <common/color.h>
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>

class Object;
typedef struct {
	double t;
	const Object *object;
} hit_t;

#include <limits>
#define DOUBLE_INF std::numeric_limits<double>::infinity()

#define HIT(t, object) (hit_t){t, object}
#define HIT_NONE HIT(DOUBLE_INF, 0)

class World;
class Object {
public:
	Object(Object *parent = 0, const point3d_t p = POINT3D_ZERO, const point3d_t d = POINT3D_ZERO);
	virtual ~Object();
	virtual const World *world() const;
	virtual World *world();
	virtual void setPose(const point3d_t p, const point3d_t d = POINT3D_ZERO);
	virtual point3d_t getPosition() const;
	virtual point3d_t getDirection() const;
	virtual hit_t hit(const ray_t *ray) const;
	virtual color_t trace(const ray_t *ray, const double t) const;
	virtual point3d_t calculate(const point3d_t point = POINT3D_ZERO) const;

protected:
	Object* parent;
	point3d_t p, d;
	std::vector<Object*> children;
	boost::numeric::ublas::matrix<double> t;
};

#endif
