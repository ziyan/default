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

#ifndef ZIYAN_RAYTRACER_COMMON_POINT3D_H
#define ZIYAN_RAYTRACER_COMMON_POINT3D_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

typedef struct {
	double x, y, z;
} point3d_t;

#define POINT3D(x,y,z) (point3d_t){x,y,z}
#define POINT3D_ZERO POINT3D(0.0,0.0,0.0)

#endif

