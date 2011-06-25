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

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <projection/image.h>

#define L_MAX 10000.0 // 1.0 or 10000.0
#define L_D_MAX 100.0
#define REINHARD
#define REINHARD_A 0.18
//#define WARD

double luminance(const Image *img) {
	double lwa = 0.0;
	const color_t *pixel = img->getPixel();
	for(int i = 0; i < img->getWidth() * img->getHeight(); i++) {
		lwa += log(0.0000001 + 0.27 * pixel->r + 0.67 * pixel->g + 0.06 * pixel->b);
		pixel++;
	}
	lwa /= (double)(img->getWidth() * img->getHeight());
	lwa = pow(M_E, lwa);
	return lwa;
}

void ward(Image *img) {
	double lwa = luminance(img);	
	double sf = pow((1.219 + pow(L_D_MAX / 2.0, 0.4)) / (1.219 + pow(lwa, 0.4)), 2.5);
	color_t *pixel = img->getPixel();
	for(int i = 0; i < img->getWidth() * img->getHeight(); i++) {
		pixel->r *= sf;
		pixel->g *= sf;
		pixel->b *= sf;
		pixel++;
	}
}

void reinhard(Image *img) {
	double lwa = luminance(img);
	color_t *pixel = img->getPixel();
	for(int i = 0; i < img->getWidth() * img->getHeight(); i++) {
		pixel->r *= REINHARD_A / lwa;
		pixel->g *= REINHARD_A / lwa;
		pixel->b *= REINHARD_A / lwa;
		pixel->r /= 1.0 + pixel->r;
		pixel->g /= 1.0 + pixel->g;
		pixel->b /= 1.0 + pixel->b;
		pixel->r *= L_D_MAX;
		pixel->g *= L_D_MAX;
		pixel->b *= L_D_MAX;
		pixel++;
	}
}

void hdr(Image *img) {
	color_t *pixel = img->getPixel();
	for(int i = 0; i < img->getWidth() * img->getHeight(); i++) {
		pixel->r *= L_MAX;
		pixel->g *= L_MAX;
		pixel->b *= L_MAX;
		pixel++;
	}
}

void compress(Image *img) {
	color_t *pixel = img->getPixel();
	for(int i = 0; i < img->getWidth() * img->getHeight(); i++) {
		pixel->r /= L_D_MAX;
		pixel->g /= L_D_MAX;
		pixel->b /= L_D_MAX;
		if(pixel->r > 1.0) pixel->r = 1.0;
		if(pixel->g > 1.0) pixel->g = 1.0;
		if(pixel->b > 1.0) pixel->b = 1.0;
		pixel++;
	}
}

int main(int argc, char** argv) {
	if(argc!=3) {
		fprintf(stderr, "Usage: %s <input-image> <output-image>\n", argv[0]);
		return -1;
	}
	Image *img = Image::load(argv[1]);
	if(!img) {
		fprintf(stderr, "Failed to load image\n");
		delete img;
		return -2;
	}
	hdr(img);
	#ifdef WARD
	ward(img);
	#endif
	#ifdef REINHARD
	reinhard(img);
	#endif
	compress(img);
	Image::save(img, argv[2]);
	delete img;
	return 0;
}

