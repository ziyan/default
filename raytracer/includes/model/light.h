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

#ifndef ZIYAN_RAYTRACER_MODEL_LIGHT_H
#define ZIYAN_RAYTRACER_MODEL_LIGHT_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <model/object.h>

typedef struct {
	color_t color;
} light_t;

#define LIGHT(color) (light_t){color}
#define LIGHT_DEFAULT LIGHT(COLOR_WHITE)

class Light : public Object {
public:
	Light(Object* parent = 0, const point3d_t p = POINT3D_ZERO, const light_t light = LIGHT_DEFAULT);
	virtual ~Light();
	virtual void setLight(const light_t light);
	virtual light_t getLight() const;
	virtual point3d_t getSource() const;
	virtual void setPose(const point3d_t p, const point3d_t d = POINT3D_ZERO);

protected:
	light_t light;
	point3d_t s;
};

#endif
