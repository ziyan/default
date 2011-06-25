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

#ifndef ZIYAN_RAYTRACER_PROJECTION_IMAGE_H
#define ZIYAN_RAYTRACER_PROJECTION_IMAGE_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <common/color.h>

class Image {
public:
	Image(const int width, const int height);
	virtual ~Image();
	virtual int getWidth() const;
	virtual int getHeight() const;
	virtual color_t* getPixel(const int h = 0, const int w = 0);
	virtual const color_t* getPixel(const int h = 0, const int w = 0) const;
	static void save(const Image *image, const char* filename);
	static Image* load(const char* filename);
protected:
	const int width, height;
	color_t* data;
};

#endif
