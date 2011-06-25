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

#ifndef ZIYAN_RAYTRACER_COMMON_COLOR_H
#define ZIYAN_RAYTRACER_COMMON_COLOR_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

typedef struct {
	double r, g, b;
} color_t;

#define COLOR(r,g,b) (color_t){r, g, b}
#define COLOR_DEFAULT COLOR(0.0,0.0,0.0)
#define COLOR_BLACK COLOR(0.0,0.0,0.0)
#define COLOR_RED COLOR(1.0,0.0,0.0)
#define COLOR_GREEN COLOR(0.0,1.0,0.0)
#define COLOR_BLUE COLOR(0.0,0.0,1.0)
#define COLOR_YELLOW COLOR(1.0,1.0,0.0)
#define COLOR_WHITE COLOR(1.0,1.0,1.0)

#endif

