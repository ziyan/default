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

#include <shapes/triangle.h>

#include <common/utils.h>

#define SAME_SIDE(p1, p2, a, b) dot_prod(cross_prod(b - a, p1 - a), cross_prod(b - a, p2 -a)) >= 0.0


Triangle::Triangle(Object *parent, const material_t material, const point3d_t a, const point3d_t b, const point3d_t c) :
	Shape(parent, material, POINT3D_ZERO, POINT3D_ZERO), a(a), b(b), c(c) {
	this->setVertices(a, b, c);
}

Triangle::~Triangle() {
}

void Triangle::setVertices(const point3d_t a, const point3d_t b, const point3d_t c) {
	this->a = a;
	this->b = b;
	this->c = c;
	point3d_t pa = calculate(a);
	point3d_t pb = calculate(b);
	point3d_t pc = calculate(c);
	va = vector3d(pa.x, pa.y, pa.z);
	vb = vector3d(pb.x, pb.y, pb.z);
	vc = vector3d(pc.x, pc.y, pc.z);
	boost::numeric::ublas::vector<double> vac = vc - va;
	boost::numeric::ublas::vector<double> vab = vb - va;
	boost::numeric::ublas::vector<double> vn = normalize(cross_prod(vab, vac));
	n = POINT3D(vn(0), vn(1), vn(2));
}

void Triangle::setPose(const point3d_t p, const point3d_t d) {
	Object::setPose(POINT3D_ZERO, POINT3D_ZERO);
	this->setVertices(a, b, c);
}

point3d_t Triangle::getVertexA() const {
	return a;
}

point3d_t Triangle::getVertexB() const {
	return b;
}

point3d_t Triangle::getVertexC() const {
	return c;
}

hit_t Triangle::hit(const ray_t *ray) const {
	boost::numeric::ublas::vector<double> vac = vc - va;
	if(dot_prod(vac, vector3d(ray->d.x, ray->d.y, ray->d.z)) == 0.0) return HIT_NONE;
	boost::numeric::ublas::vector<double> vn = vector3d(n.x, n.y, n.z);
	double a = vn(0);
	double b = vn(1);
	double c = vn(2);
	// f = n . a
	double f = dot_prod(vn, va);
	double t = (f-a*ray->p.x-b*ray->p.y-c*ray->p.z)/(a*ray->d.x+b*ray->d.y+c*ray->d.z);
	boost::numeric::ublas::vector<double> vp = vector3d(t*ray->d.x+ray->p.x, t*ray->d.y+ray->p.y, t*ray->d.z+ray->p.z);
	if(!(SAME_SIDE(vp, va, vb, vc) && SAME_SIDE(vp, vb, va, vc) && SAME_SIDE(vp, vc, va, vb))) return HIT_NONE;
	return HIT(t, this);
}

color_t Triangle::trace(const ray_t *ray, const double t) const {
	point3d_t p = POINT3D(ray->p.x + ray->d.x * t, ray->p.y + ray->d.y * t, ray->p.z + ray->d.z * t);
	// TODO: need to calculate texture coordinate for real
	return this->phong(p, POINT2D(p.x, p.z), n, ray);
}
