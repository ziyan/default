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

#include <projection/camera.h>

#include <cmath>
#include <vector>
#include <stdio.h>
#include <common/utils.h>

Camera::Camera(const World *world, const point3d_t eye, const point3d_t lookat, const point3d_t up, const double fov, const double near, const double far, const int ttl) :
	world(world), eye(eye), lookat(lookat), up(up), fov(fov), near(near), far(far), ttl(ttl) {
	this->setPose(eye, lookat, up);
}

Camera::~Camera() {
}

void Camera::setPose(const point3d_t eye, const point3d_t lookat, const point3d_t up) {
	this->eye = eye;
	this->lookat = lookat;
	this->up = up;
	// n = eye - lookat
	n = normalize(vector3d(lookat.x - eye.x, lookat.y - eye.y, lookat.z - eye.z));
	// u = up x n
	u = normalize(cross_prod(normalize(vector3d(up.x,up.y,up.z)), n));
	// v = n x u
	v = normalize(cross_prod(n, u));
}

point3d_t Camera::getEye() const {
	return eye;
}

point3d_t Camera::getLookAt() const {
	return lookat;
}

point3d_t Camera::getUp() const {
	return up;
}

double Camera::getFOV() const {
	return fov;
}

double Camera::getNear() const {
	return near;
}

double Camera::getFar() const {
	return far;
}

void Camera::setFOV(double fov) {
	this->fov = fov;
}

void Camera::setNear(double near) {
	this->near = near;
}

void Camera::setFar(double far) {
	this->far = far;
}

void Camera::snap(Image *film) const {
	if(!film) return;
	double width = (double)film->getWidth();
	double height = (double)film->getHeight();
	double width2 = (width-1.0)/2.0;
	double height2 = (height-1.0)/2.0;

	// calculate focal length
	double f = height / 2.0 / tan(fov / 180.0 * M_PI / 2.0);

	// generate rays
	for(int h = 0; h < height; h++) {
		for(int w = 0; w < width; w++) {
			// ray vector
			boost::numeric::ublas::vector<double> rayv(3);
			rayv = -((double)w-width2) * u + -((double)h-height2) * v + f * n;
			// normalize
			rayv = normalize(rayv);

			ray_t ray;
			ray.p = POINT3D(eye.x, eye.y, eye.z);
			ray.d = POINT3D(rayv(0), rayv(1), rayv(2));
			ray.near = near;
			ray.far = far;
			ray.ttl = ttl;
			*(film->getPixel(h, w)) = world->trace(&ray);
			//std::cout << "." << std::flush;
		}
		std::cout << h << " " << std::flush;
	}
	std::cout << std::endl;
}
