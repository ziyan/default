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
#include <stdint.h>

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
	 * @param depth depth of the map in pixels, when > 0, 3D texture will be built
	 * @param mpp resolution of the map, in meter/pixel
	 * @param parent parent QObject
	 */
	Map(const char* name,
		const int width,
		const int height,
		const int depth,
		const double mpp,
		QObject *parent = 0);

	/**
	 * Destructor
	 */
	virtual ~Map();

	/// data accessors
	virtual int getWidth() const;
	virtual int getHeight() const;
	virtual int getDepth() const;
	virtual double getMPP() const;
	virtual int8_t getValue(const int x, const int y) const;
	virtual uint8_t *getTexture() const;

	/// mapping functions
	virtual void mapLaser(const double x,
		const double y,
		const double yaw,
		const double ranges[361]);
	virtual void mapFrame(const double x,
		const double y,
		const double z,
		const double pitch,
		const double yaw,
		const double hfov,
		const double vfov,
		const int frame_width,
		const int frame_height,
		const uint8_t *frame);


	/// map static tools
	static void save(const Map* map);
	static Map *load(const char* filename);

protected:
	/**
	 * Internal static function to normalize an angle in radian
	 * @param angle
	 * @return
	 */
	static double normalize(const double angle);

	/**
	 * Internal static function to model a laser beam (used by <code>mapLaser</code>)
	 * @param range the laser beam measured range
	 * @param a the angle deviation from the direction of the laser beam
	 * @param d the distance from the origin of the laser beam
	 * @return
	 */
	static double modelLaser(const double range, const double a, const double d);

	std::string name; // name of the map
	double mpp; // resolution of the map in meters/pixel
	int width, height, depth; // dimension of the map, depth is used by 3D texture
	int8_t *data; // occupancy data of the map
	uint8_t *texture; // 3D texture of the map
};

#endif //SLAM_MODEL_MAP_H

