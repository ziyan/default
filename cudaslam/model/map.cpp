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
#include "../cuda/map.h"
#include <iostream>
#include <fstream>
#include <QImage>
#include <QtDebug>

using namespace std;

Map::Map(const char* name, const int width, const int height, const float mpp, QObject *parent) :
	QObject(parent),
	name(name),
	mpp(mpp), // 0.082
	width(width), //2048
	height(height), //1024
	data(new char[width*height]),
	map(0)
{
	memset(data, 120, sizeof(char) * width * height);

	// allocate map
	cuda_map_create(&map, width, height);
	cuda_map_sync_to_gpu(map, data, width, height);
}

Map::~Map() {
	delete [] data;
	cuda_map_free(&map);
}


/// accessors for data

int Map::getWidth() const {
	return width;
}

int Map::getHeight() const {
	return height;
}

float Map::getMPP() const {
	return mpp;
}

int8_t Map::getValue(const int x, const int y) const {
	return data[y * width + x];
}

void Map::sync() {
	cuda_map_sync_to_cpu(map, data, width, height);
}



void Map::mapLaser(const float x, const float y, const float yaw, const float *ranges) {
	cuda_map_laser(map, width, height, mpp, x, y, yaw, ranges);
}

/// map static tools

void Map::save(Map* map) {
	if(!map) return;
	{
		ofstream out(map->name.c_str());
		out << map->width << endl;
		out << map->height << endl;
		out << map->mpp << endl;
		out.flush();
		out.close();
	}
	map->sync();
	{
		string data = map->name + ".data";
		ofstream out(data.c_str());
		out.write((const char*)map->data, map->width*map->height);
		out.flush();
		out.close();
	}
}

Map* Map::load(const char* filename) {
	int width, height;
	float mpp;
	{
		ifstream in(filename);
		if(!in) return 0;
		in >> width >> height >> mpp;
		if(!in) return 0;
		in.close();
	}
	Map *map = new Map(filename, width, height, mpp);
	{
		string data(filename); data +=".data";
		ifstream in(data.c_str());
		if(!in) { delete map; return 0; }
		in.read(map->data, map->width*map->height);
		if(!in) { delete map; return 0; }
		in.close();
		cuda_map_sync_to_gpu(map->map, map->data, width, height);
	}
	return map;
}

