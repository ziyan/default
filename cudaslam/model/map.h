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

#ifndef SLAM_MODEL_MAP_H
#define SLAM_MODEL_MAP_H

#include <QObject>
#include <string>

#define MAP_TRAVERSABLE -100
#define MAP_INTRAVERSABLE 100
#define MAP_UNKNOWN 0
#define MAP_UNEXPLORED 120


/**
 * Class Map
 * Represents a occupancy gridmap, values are stored as signed char
 */
class Map : public QObject {
	Q_OBJECT
public:
	/**
	 * Constructor for map
	 * @param name name of the map, filename to save
	 * @param width width of the map in pixels
	 * @param height height of the map in pixels
	 * @param mpp resolution of the map, in meter/pixel
	 * @param parent parent QObject
	 */
	Map(const char* name,
		const int width,
		const int height,
		const float mpp,
		QObject *parent = 0);

	/**
	 * Destructor
	 */
	virtual ~Map();

	/// data accessors
	virtual int getWidth() const;
	virtual int getHeight() const;
	virtual float getMPP() const;
	virtual int8_t getValue(const int x, const int y) const;

public Q_SLOTS:
	virtual void mapLaser(const float x, const float y, const float yaw, const float *ranges);
	virtual void sync();

public:
	/// map static tools
	static void save(Map* map);
	static Map *load(const char* filename);

protected:
	std::string name; // name of the map
	float mpp; // resolution of the map in meters/pixel
	int width, height; // dimension of the map, depth is used by 3D texture
	char *data;
	void *map;
};

#endif //SLAM_MODEL_MAP_H

