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

#include <model/light.h>

#include <model/world.h>

Light::Light(Object* parent, const point3d_t p, const light_t light) :
	Object(parent, p), light(light), s(calculate(POINT3D_ZERO)) {
	World *world = this->world();
	if (world) world->lights.push_back(this);
}

Light::~Light() {
	World *world = this->world();
	if(world && find(world->lights.begin(), world->lights.end(), this) != world->lights.end())
		world->lights.erase(find(world->lights.begin(), world->lights.end(), this));
}

void Light::setLight(const light_t light) {
	this->light = light;
}

light_t Light::getLight() const {
	return light;
}

point3d_t Light::getSource() const {
	return s;
}

void Light::setPose(const point3d_t p, const point3d_t d) {
	Object::setPose(p, d);
	s = calculate(POINT3D_ZERO);
}
