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

#include "world.h"
#include <QApplication>

World::World(Map *map, QObject *parent) :
	QObject(parent), map(map) {
	connect(QApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(save()));
}

World::~World() {
}

void World::save() {
	qDebug("World: saving map ...");
	Map::save(map);
	qDebug("World: map saved.");
}

Map *World::getMap() {
	return map;
}

