/*
 * Copyright (C) 2009 Ziyan Zhou <zhou@ziyan.info>
 * Visit http://ziyan.info/tag/ritcg2slam/ for more information
 *
 * This file is part of SLAM Visualization for Player.
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "map.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <QImage>

using namespace std;

Map::Map(const char* name, const int width, const int height, const int depth, const double mpp, QObject *parent) :
	QObject(parent),
	name(name),
	mpp(mpp), // 0.082
	width(width), //2048
	height(height), //1024
	depth(depth),
	data(new int8_t[width*height]),
	texture(0) {
	// allocate map
	for(int8_t* ptr = data; ptr != data+width*height; ptr++)
		*ptr = MAP_UNEXPLORED;
	if(depth > 0) {
		texture = new uint8_t[width*height*depth*3];
		// floor
		for(uint8_t* ptr = texture; ptr != texture+width*height*3; ptr++)
			*ptr = rand() % 10 + 10;
		for(uint8_t* ptr = texture + width*height*3; ptr != texture+width*height*depth*3; ptr++)
			*ptr = rand() % 10 + 90;
	}
}

Map::~Map() {
	delete [] data;
	if(texture) delete [] texture;
}


/// accessors for data

int Map::getWidth() const {
	return width;
}

int Map::getHeight() const {
	return height;
}

int Map::getDepth() const {
	return depth;
}

double Map::getMPP() const {
	return mpp;
}

int8_t Map::getValue(const int x, const int y) const {
	return data[y * width + x];
}

uint8_t *Map::getTexture() const {
	return texture;
}


#define LASER_RANGE_MAX 8.0
#define LASER_RESOLUTION (M_PI / 360.0)

void Map::mapLaser(const double x, const double y, const double yaw, const double ranges[361]) {
	double bearing = normalize(yaw);

	// calculate bounding box
	int left = (int)floor((x - LASER_RANGE_MAX) / mpp);
	int right = (int)ceil((x + LASER_RANGE_MAX) / mpp);
	int top = (int)floor((y - LASER_RANGE_MAX) / mpp);
	int bottom = (int)ceil((y + LASER_RANGE_MAX) / mpp);

	for(int w = left; w <= right; w++) {
		for(int h = top; h <= bottom; h++) {
			// convert to map coords
			int ww = w + width / 2;
			int hh = h + height / 2;
			if (ww < 0 || hh < 0 || ww >= width || hh >= height) continue;
			double dw = mpp * w;
			double dh = mpp * h;
			double t = sqrt((x-dw)*(x-dw)+(y-dh)*(y-dh));
			if (t > LASER_RANGE_MAX) continue;
			double da = normalize(atan2(dh - y, dw - x));
			int i = (int)ceil(normalize(da - bearing - LASER_RESOLUTION / 2.0) / LASER_RESOLUTION) + 180;
			if(i < 0 || i >= 361) continue;

			double change = modelLaser(ranges[i], normalize(da - (bearing + LASER_RESOLUTION * (i - 180))), t);
			if(change != 0.0) {
				int8_t &value = data[hh * width +ww];
				if(value == MAP_UNEXPLORED) value = MAP_UNKNOWN;
				double prob = (double)value / 100.0 + change;
				if(prob > 1.0) prob = 1.0;
				if(prob < -1.0) prob = -1.0;
				value = 100 * prob;
				if(value < MAP_TRAVERSABLE) value = MAP_TRAVERSABLE;
				if(value > MAP_INTRAVERSABLE) value  = MAP_INTRAVERSABLE;
			}
		}
	}
}

#define CAMERA_EFFECTIVE_DISTANCE 20.0 // used to calculate bounding box
#define TEXTURE_UPDATE_RATE 1.0

void Map::mapFrame(const double x,
	const double y,
	const double z,
	const double pitch,
	const double yaw,
	const double hfov,
	const double vfov,
	const int frame_width,
	const int frame_height,
	const uint8_t *frame) {

	if(depth <= 0 || !texture) return;

	// calculate bounding box
	int x1 = (int)((x + CAMERA_EFFECTIVE_DISTANCE * cos(yaw + hfov / 2.0)) / mpp);
	int x2 = (int)((x + CAMERA_EFFECTIVE_DISTANCE * cos(yaw - hfov / 2.0)) / mpp);
	int y1 = (int)((y + CAMERA_EFFECTIVE_DISTANCE * sin(yaw + hfov / 2.0)) / mpp);
	int y2 = (int)((y + CAMERA_EFFECTIVE_DISTANCE * sin(yaw - hfov / 2.0)) / mpp);

	int xMin = (x1 < x2) ? x1 : x2;
	xMin = ((int)(x / mpp) < xMin) ? (int)(x / mpp) : xMin;
	int yMin = (y1 < y2) ? y1 : y2;
	yMin = ((int)(y / mpp) < yMin) ? (int)(y / mpp) : yMin;
	int xMax = (x1 > x2) ? x1 : x2;
	xMax = ((int)(x / mpp) > xMax) ? (int)(x / mpp) : xMax;
	int yMax = (y1 > y2) ? y1 : y2;
	yMax = ((int)(y / mpp) > yMax) ? (int)(y / mpp) : yMax;

	for(int w = xMin; w <= xMax; w++) {
		for(int h = yMin; h <= yMax; h++) {
			// convert to map coordinates
			int ww = w + width / 2;
			int hh = h + height / 2;
			if (ww < 0 || hh < 0 || ww >= width || hh >= height) continue;
			//if (data[hh * width +ww] == MAP_UNEXPLORED) continue;
			double dw = mpp * w;
			double dh = mpp * h;
			double t = sqrt((x-dw)*(x-dw)+(y-dh)*(y-dh));
			if (t > CAMERA_EFFECTIVE_DISTANCE) continue;

			double dyaw = normalize(normalize(atan2(dh - y, dw - x)) - yaw);
			if (fabs(dyaw) > hfov / 2.0) continue;

			// frame coordinates
			int fx = frame_width - (int)((frame_width / 2) * (dyaw / (hfov / 2.0))) + (frame_width / 2);

			for(int d = 0; d < depth; d++) {
				double dd = mpp * d;
				double dpitch = normalize(- normalize(atan2(z - dd, t)) - pitch);
				if(fabs(dpitch) > vfov / 2.0) continue;
				// frame coordinates
				int fy = (int)(-(frame_height / 2) * (dpitch / (vfov / 2.0))) + (frame_height / 2);

				// base indicies into the data arrays
				int ti = width * height * 3 * d + hh * width * 3 + ww * 3;
				int fi = fy * frame_width * 3 + fx * 3;

				texture[ti + 0] = (int)((1 - TEXTURE_UPDATE_RATE) * texture[ti + 0] + TEXTURE_UPDATE_RATE * frame[fi + 0]);
				texture[ti + 1] = (int)((1 - TEXTURE_UPDATE_RATE) * texture[ti + 1] + TEXTURE_UPDATE_RATE * frame[fi + 1]);
				texture[ti + 2] = (int)((1 - TEXTURE_UPDATE_RATE) * texture[ti + 2] + TEXTURE_UPDATE_RATE * frame[fi + 2]);
			}
		}
	}
}

/// map static tools

void Map::save(const Map* map) {
	if(!map) return;
	{
		ofstream out(map->name.c_str());
		out << map->width << endl;
		out << map->height << endl;
		out << map->depth << endl;
		out << map->mpp << endl;
		out.flush();
		out.close();
	}
	{
		string data = map->name + ".data";
		ofstream out(data.c_str());
		out.write((const char*)map->data, map->width*map->height);
		out.flush();
		out.close();
	}
	if(map->depth > 0 && map->texture)
	{
		string texture = map->name + ".texture";
		ofstream out(texture.c_str());
		out.write((const char*)map->texture, map->width*map->height*map->depth*3);
		out.flush();
		out.close();
	}
}

Map* Map::load(const char* filename) {
	int width, height, depth;
	double mpp;
	{
		ifstream in(filename);
		if(!in) return 0;
		in >> width >> height >> depth >> mpp;
		if(!in) return 0;
		in.close();
	}
	Map *map = new Map(filename, width, height, depth, mpp);
	{
		string data(filename); data +=".data";
		ifstream in(data.c_str());
		if(!in) { delete map; return 0; }
		in.read((char*)map->data, map->width*map->height);
		if(!in) { delete map; return 0; }
		in.close();
	}
	if(map->depth > 0 && map->texture)
	{
		string texture(filename); texture +=".texture";
		ifstream in(texture.c_str());
		if(!in) { delete map; return 0; }
		in.read((char*)map->texture, map->width*map->height*map->depth*3);
		if(!in) { delete map; return 0; }
		in.close();
	}
	return map;
}

double Map::normalize(const double a) {
	double angle = a;
	while(angle < -M_PI) angle += M_PI + M_PI;
	while(angle >= M_PI) angle -= M_PI + M_PI;
	return angle;
}

#define D1 0.2
#define D2 0.25
#define D3 0.3

double Map::modelLaser(const double range, const double a, const double d) {
	double g =  -0.005 * (range > LASER_RANGE_MAX ? LASER_RANGE_MAX : range) + 0.05;
	double n = 1.0 / (0.05 * 2.0 * M_PI) * exp(a*a/-2.0);
	double s = g * n;
	if ( d < range - D1 ) {
		return -s;
	} else if ( d < range + D1 ) {
		return -s + s / D1 * ( d - range + D1 );
	} else if ( d < range + D2 ) {
		return s;
	} else if ( d < range + D3 ) {
		return s - s / (D3 - D2) * ( d - range - D2 );
	} else {
		return 0.0;
	}
}


