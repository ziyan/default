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

#ifndef ZIYAN_RAYTRACER_PROJECTION_CAMERA_H
#define ZIYAN_RAYTRACER_PROJECTION_CAMERA_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <projection/image.h>
#include <model/world.h>
#include <boost/numeric/ublas/vector.hpp>

class Camera {
public:
	Camera(const World *world, const point3d_t eye, const point3d_t lookat, const point3d_t up, const double fov, const double near, const double far, const int ttl);
	virtual ~Camera();
	virtual void snap(Image *film) const;
	virtual void setPose(const point3d_t eye, const point3d_t lookat, const point3d_t up);
	virtual point3d_t getEye() const;
	virtual point3d_t getLookAt() const;
	virtual point3d_t getUp() const;
	virtual double getFOV() const;
	virtual double getNear() const;
	virtual double getFar() const;
	virtual void setFOV(double fov);
	virtual void setNear(double near);
	virtual void setFar(double far);

protected:
	const World *world;
	point3d_t eye, lookat, up;
	double fov, near, far;
	int ttl;
	boost::numeric::ublas::vector<double> n, v, u;
};

#endif

