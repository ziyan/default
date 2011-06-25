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

#include <model/object.h>

#include <algorithm>
#include <cmath>
#include <common/utils.h>
#include <boost/numeric/ublas/matrix.hpp>


Object::Object(Object *parent, const point3d_t p, const point3d_t d) :
	parent(parent), p(p), d(d), t(4,4) {
	if(parent)
		parent->children.push_back(this);
	this->setPose(p, d);
}

Object::~Object() {
	if(parent && find(parent->children.begin(), parent->children.end(), this) != parent->children.end())
		parent->children.erase(find(parent->children.begin(), parent->children.end(), this));
	for(std::vector<Object*>::iterator i = children.begin(); i!=children.end();i++) {
		(*i)->parent = 0; // make children an orphan first
		delete (*i); // then delete it
	}
}

const World *Object::world() const {
	if(parent)
		return parent->world();
	return 0;
}

World *Object::world() {
	if(parent)
		return parent->world();
	return 0;
}

void Object::setPose(const point3d_t p, const point3d_t d) {
	this->p = p;
	this->d = d;

	double c, s;
	// rotate about x
	c = cos(d.x);
	s = sin(d.x);
	boost::numeric::ublas::matrix<double> rx(4, 4);
	rx(0,0) = 1.0;	rx(0,1) = 0.0;	rx(0,2) = 0.0;	rx(0,3) = 0.0;
	rx(1,0) = 0.0;	rx(1,1) = c;	rx(1,2) = -s;	rx(1,3) = 0.0;
	rx(2,0) = 0.0;	rx(2,1) = s;	rx(2,2) = c;	rx(2,3) = 0.0;
	rx(3,0) = 0.0;	rx(3,1) = 0.0;	rx(3,2) = 0.0;	rx(3,3) = 1.0;

	// rotate about y
	c = cos(d.y);
	s = sin(d.y);
	boost::numeric::ublas::matrix<double> ry(4, 4);
	ry(0,0) = c;	ry(0,1) = 0.0;	ry(0,2) = s;	ry(0,3) = 0.0;
	ry(1,0) = 0.0;	ry(1,1) = 1.0;	ry(1,2) = 0.0;	ry(1,3) = 0.0;
	ry(2,0) = -s;	ry(2,1) = 0.0;	ry(2,2) = c;	ry(2,3) = 0.0;
	ry(3,0) = 0.0;	ry(3,1) = 0.0;	ry(3,2) = 0.0;	ry(3,3) = 1.0;

	// rotate about z
	c = cos(d.z);
	s = sin(d.z);
	boost::numeric::ublas::matrix<double> rz(4, 4);
	rz(0,0) = c;	rz(0,1) = -s;	rz(0,2) = 0.0;	rz(0,3) = 0.0;
	rz(1,0) = s;	rz(1,1) = c;	rz(1,2) = 0.0;	rz(1,3) = 0.0;
	rz(2,0) = 0.0;	rz(2,1) = 0.0;	rz(2,2) = 1.0;	rz(2,3) = 0.0;
	rz(3,0) = 0.0;	rz(3,1) = 0.0;	rz(3,2) = 0.0;	rz(3,3) = 1.0;

	// translation
	boost::numeric::ublas::matrix<double> tt(4, 4);
	tt(0,0) = 1.0;	tt(0,1) = 0.0;	tt(0,2) = 0.0;	tt(0,3) = p.x;
	tt(1,0) = 0.0;	tt(1,1) = 1.0;	tt(1,2) = 0.0;	tt(1,3) = p.y;
	tt(2,0) = 0.0;	tt(2,1) = 0.0;	tt(2,2) = 1.0;	tt(2,3) = p.z;
	tt(3,0) = 0.0;	tt(3,1) = 0.0;	tt(3,2) = 0.0;	tt(3,3) = 1.0;

	// combine
	t = prod(ry, rx);
	t = prod(rz, t);
	t = prod(tt, t);

	// signal all children to update
	for(std::vector<Object*>::iterator i = children.begin(); i!=children.end();i++)
		(*i)->setPose((*i)->p, (*i)->d);
}

point3d_t Object::getPosition() const {
	return p;
}

point3d_t Object::getDirection() const {
	return d;
}

hit_t Object::hit(const ray_t *ray) const {
	hit_t hit = HIT_NONE;
	for(std::vector<Object*>::const_iterator i = children.begin(); i!=children.end(); i++) {
		hit_t h = (*i)->hit(ray);
		if(h.object && h.t >= ray->near && h.t < hit.t && h.t <= ray->far)
			hit = h;
	}
	return hit;
}

color_t Object::trace(const ray_t *ray, const double t) const {
	return COLOR_DEFAULT;
}


point3d_t Object::calculate(const point3d_t point) const {
	boost::numeric::ublas::vector<double> v(4);
	v(0) = point.x; v(1) = point.y; v(2) = point.z; v(3) = 1.0;
	v = prod(t, v);
	if(parent)
		return parent->calculate(POINT3D(v(0),v(1),v(2)));
	return POINT3D(v(0),v(1),v(2));
}
