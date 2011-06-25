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

#include <projection/image.h>

#include <iostream>
#include <fstream>
#include <string>

Image::Image(const int width, const int height) : width(width), height(height) {
	data = new color_t[width*height];
}

Image::~Image() {
	delete [] data;
}

int Image::getWidth() const {
	return width;
}

int Image::getHeight() const {
	return height;
}

color_t* Image::getPixel(const int h, const int w) {
	if(w < width && w >= 0 && h < height && h >= 0)
		return &data[h * width + w];
	return 0;
}

const color_t* Image::getPixel(const int h, const int w) const {
	if(w < width && w >= 0 && h < height && h >= 0)
		return &data[h * width + w];
	return 0;
}

void Image::save(const Image *image, const char* filename) {
	if(!image) return;
	std::ofstream out(filename);
	out << "P3" << std::endl;
	out << "# Ziyan Zhou Ray Tracer Generated Image" << std::endl;
	out << image->width << " " << image->height << std::endl;
	out << "255" << std::endl;
	color_t *pixel = image->data;
	for (int i = 0; i < image->width*image->height; ++i) {
		out << (int)(255.0 * pixel->r) << " ";
		out << (int)(255.0 * pixel->g) << " ";
		out << (int)(255.0 * pixel->b) << " ";
		pixel++;
	}
	out << std::endl;
	out.close();
}

Image *Image::load(const char* filename) {
	std::ifstream in(filename);
	if(!in) {
		in.close();
		return 0;
	}
	int width, height;
	int value;
	char version[256], comment[256];
	in.getline(version, 256);
	if(!in) {
		in.close();
		return 0;
	}
	in.getline(comment, 256);
	if(!in) {
		in.close();
		return 0;
	}
	in >> width >> height >> value;
	if(!in) {
		in.close();
		return 0;
	}
	Image *image = new Image(width, height);
	color_t *pixel = image->data;
	int r, g, b;
	//printf("%s %i %i %i\n", comment, width, height, value);
	for(int i = 0; i<width*height; i++) {
		in >> r >> g >> b;
		*pixel = COLOR((double)r/(double)value,(double)g/(double)value,(double)b/(double)value);
		pixel++;
	}
	if(!in) {
		delete image;
		in.close();
		return 0;
	}
	in.close();
	return image;
}

