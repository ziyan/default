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

#ifndef ZIYAN_RAYTRACER_MODEL_WORLD_H
#define ZIYAN_RAYTRACER_MODEL_WORLD_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <model/object.h>

class Light;
class World : public Object {
public:
	World(const color_t background = COLOR_BLACK, const color_t ambient = COLOR_WHITE);
	virtual ~World();
	virtual const World *world() const;
	virtual World *world();
	virtual void setBackground(const color_t background);
	virtual void setAmbient(const color_t ambient);
	virtual color_t getBackground() const;
	virtual color_t getAmbient() const;
	virtual color_t trace(const ray_t *ray) const;

	std::vector<Light*> lights;
	color_t background, ambient;
};

#endif

