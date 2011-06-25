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

#ifndef ZIYAN_RAYTRACER_MODEL_SHAPE_H
#define ZIYAN_RAYTRACER_MODEL_SHAPE_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <model/object.h>

#include <common/point2d.h>

typedef struct {
	color_t color, specular;
	double ka, kd, ks, ke, kr, kt, n;
	color_t (*texture)(point2d_t);
} material_t;

#define MATERIAL(color) (material_t){color,COLOR_WHITE,0.5,0.3,0.3,10.0,0.0,0.0,0.0,0}
#define MATERIAL_DEFAULT MATERIAL(COLOR_RED)

class Shape : public Object {
public:
	Shape(Object *parent = 0, const material_t material = MATERIAL_DEFAULT, const point3d_t p = POINT3D_ZERO, const point3d_t d = POINT3D_ZERO);
	virtual ~Shape();
	virtual void setMaterial(const material_t material);
	virtual material_t getMaterial() const;
	virtual color_t phong(const point3d_t p, const point2d_t tc, const point3d_t n, const ray_t* ray) const;
protected:
	material_t material;
};

#endif
