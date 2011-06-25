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

#include <model/world.h>

World::World(const color_t background, const color_t ambient) : Object(), background(background), ambient(ambient) {
}

World::~World() {
}

const World *World::world() const {
	return this;
}

World *World::world() {
	return this;
}

void World::setBackground(const color_t background) {
	this->background = background;
}

void World::setAmbient(const color_t ambient) {
	this->ambient = ambient;
}

color_t World::getBackground() const {
	return background;
}

color_t World::getAmbient() const {
	return ambient;
}

color_t World::trace(const ray_t *ray) const {
	hit_t hit = this->hit(ray);
	if(hit.object) return hit.object->trace(ray, hit.t);
	return background;
}
