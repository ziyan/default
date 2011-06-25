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

#include <shapes/sphere.h>

#include <cmath>
#include <common/utils.h>

Sphere::Sphere(Object *parent, const material_t material, const double radius, const point3d_t p) :
	Shape(parent, material, p, POINT3D_ZERO), radius(radius), c(calculate(POINT3D_ZERO)) {
}

Sphere::~Sphere() {
}

void Sphere::setRadius(const double radius) {
	this->radius = radius;
}

double Sphere::getRadius() const {
	return radius;
}

point3d_t Sphere::getCenter() const {
	return c;
}

void Sphere::setPose(const point3d_t p, const point3d_t d) {
	Object::setPose(p, d);
	c = calculate(POINT3D_ZERO);
}

hit_t Sphere::hit(const ray_t *ray) const {
	point3d_t p = c;
	double a = ray->d.x * ray->d.x + ray->d.y * ray->d.y + ray->d.z * ray->d.z;
	double b = 2.0*(ray->d.x*(ray->p.x-p.x)+ray->d.y*(ray->p.y-p.y)+ray->d.z*(ray->p.z-p.z));
	double c = (ray->p.x-p.x)*(ray->p.x-p.x) + (ray->p.y-p.y)*(ray->p.y-p.y) + (ray->p.z-p.z)*(ray->p.z-p.z) - radius*radius;
	double delta = b*b - 4.0*a*c;
	if(delta < 0.0) return HIT_NONE;
	double t;
	if(delta == 0.0)
		t = -b / (2.0 * a);
	else {
		double t1 = (-b + sqrt(delta)) / (2.0 * a);
		double t2 = (-b - sqrt(delta)) / (2.0 * a);
		if(t1 < ray->near && t2 < ray->near) return HIT_NONE;
		if(t1 < ray->near) t = t2;
		else if(t2 < ray->near) t = t1;
		else if(t1 < t2) t = t1;
		else t = t2;
	}
	return HIT(t, this);
}

color_t Sphere::trace(const ray_t *ray, const double t) const {
	point3d_t p = POINT3D(ray->p.x + ray->d.x * t, ray->p.y + ray->d.y * t, ray->p.z + ray->d.z * t);
	return this->phong(p, POINT2D_ZERO,
			POINT3D((p.x - c.x)/radius, (p.y - c.y)/radius, (p.z - c.z)/radius),
			ray);
}
