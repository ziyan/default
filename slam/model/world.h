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

#ifndef SLAM_MODEL_WORLD_H
#define SLAM_MODEL_WORLD_H

#include <QObject>
#include "map.h"

/**
 * Class World
 * At the moment, this class does not do much, simply a container
 */
class World : public QObject {
	Q_OBJECT
public:
	World(Map *map, QObject *parent = 0);
	virtual ~World();
	virtual Map *getMap();

protected Q_SLOTS:
	/**
 	 * Map saving slot, called when <code>QApplication::aboutToQuit()</code>
	 * is signaled
	 */
	virtual void save();

protected:
	Map *map;
};

#endif //SLAM_MODEL_WORLD_H

