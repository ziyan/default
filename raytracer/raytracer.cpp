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
#include <model/world.h>
#include <model/light.h>
#include <projection/camera.h>
#include <shapes/triangle.h>
#include <shapes/sphere.h>
#include <shapes/cube.h>
#include <shapes/cone.h>

Image *floor_img = 0; //Image::load("texture/water.ppm");

color_t floor_texture1(point2d_t p) {
	int x = 10 * (p.u + 0.75);
	int y = 10 * (p.v + 6.0);
	if(x % 2 == 0 && y % 2 == 0)
		return COLOR_RED;
	else if(x % 2 != 0 && y % 2 != 0)
		return COLOR_RED;
	else if(x % 2 == 0)
		return COLOR_YELLOW;
	return COLOR_YELLOW;
}

color_t floor_texture2(point2d_t p) {
	double s = sin(((p.u + 0.7) / 1.75) * M_PI * 10.0);
	double c = sin(((p.v + 6.0) / 12.0) * M_PI * 120.0);
	return COLOR(fabs(s),fabs(c),0.0);
}

color_t floor_texture3(point2d_t p) {
	int x = 1000 * (p.u + 0.7);
	int y = 1000 * (p.v + 6.0);
	color_t *c = floor_img->getPixel(y % 1716, x % 2576);
	if(c) return *c;
	return COLOR_BLACK;
}

int main(int argc, char** argv) {
	if(argc!=2) {
		std::cerr << "Usage: " << argv[0] << " <file-name>" << std::endl;
		return -1;
	}

	World world; //(COLOR(0.3, 0.3, 1.0));
	Light light1(&world, POINT3D(2.0, 3.0, 2.0));
	//Light light2(&world, POINT3D(-4.0, 5.0, 5.0), LIGHT(COLOR_YELLOW));
	//Light light3(&world, POINT3D(2.0, 0.0, 5.0));

	material_t blue_ball_mat = MATERIAL_DEFAULT, green_ball_mat = MATERIAL_DEFAULT;
	blue_ball_mat.color = COLOR_WHITE;
	green_ball_mat.color = COLOR(0.7,0.7,0.7);
	blue_ball_mat.specular = green_ball_mat.specular = COLOR_WHITE;
	blue_ball_mat.ka = 0.075; green_ball_mat.ka = 0.15;
	blue_ball_mat.kd = 0.075; green_ball_mat.kd = 0.25;
	blue_ball_mat.ks = 0.2; green_ball_mat.ks = 1.0;
	blue_ball_mat.ke = green_ball_mat.ke = 20.0;
	blue_ball_mat.kr = 0.05; green_ball_mat.kr = 0.75;
	blue_ball_mat.kt = 0.85;
	blue_ball_mat.n = 0.95;
	blue_ball_mat.texture = green_ball_mat.texture = 0;

	Sphere blue(&world, blue_ball_mat, 0.15, POINT3D(0.45, 0.3, 0.75));
	Sphere green(&world, green_ball_mat, 0.15, POINT3D(0.3, 0.2, 0.5));
	Object floor(&world);
	material_t floor_mat = MATERIAL_DEFAULT;
	floor_mat.color = COLOR_RED;
	floor_mat.specular = COLOR_WHITE;
	floor_mat.ka = 0.7;
	floor_mat.kd = 0.2;
	floor_mat.ks = 0.2;
	floor_mat.ke = 10.0;
	floor_mat.texture = floor_texture1;
	Triangle floor1(&floor, floor_mat, POINT3D(-0.7, 0.0, 1.0), POINT3D(1.05, 0.0, 1.0), POINT3D(1.05, 0.0, -1.5));
	Triangle floor2(&floor, floor_mat, POINT3D(-0.7, 0.0, 1.0), POINT3D(1.05, 0.0, -1.5), POINT3D(-0.7, 0.0, -1.5));
	//Cube cube(&world, MATERIAL(COLOR_YELLOW), POINT3D(0.4, 0.4, 0.4), POINT3D(1.0, 0.5, -0.5), POINT3D(M_PI/4.0, -M_PI / 4.0, M_PI/4.0));
	//Cone cone(&world, MATERIAL(COLOR_YELLOW), 1.0, 5, 0.5, 20, POINT3D(0.0, 0.5, -0.5), POINT3D(-M_PI/16.0,0.0,0.0));
	Camera cam(&world, POINT3D(0.5, 0.3, 1.5), POINT3D(0.5, 0.3, 0.0), POINT3D(0.0, 1.0, 0.0), 45.0, 0.01, 10.0, 10);
	Image img(1024,768);
	cam.snap(&img);
	Image::save(&img, argv[1]);
	return 0;
}
