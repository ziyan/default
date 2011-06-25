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

#include <iostream>
#include <QtGui/QApplication>
#include "model/world.h"
#include "model/robot.h"
#include "ui/glwidget.h"

using namespace std;

int main(int argc, char *argv[])
{
	if(argc < 2) {
		fprintf(stderr, "Usage: %s <map-file> [hostname] [port]\n", argv[0]);
		fprintf(stderr, "\n");
		fprintf(stderr, "Two modes are available:\n");
		fprintf(stderr, "  - recording mode, supply hostname to record sensor data from player server\n");
		fprintf(stderr, "  - playback mode, supply only the map file to playback\n");
		fprintf(stderr, "\n");
		return -1;
	}

	QApplication a(argc, argv);

	Map *map = Map::load(argv[1]);

	// shall we play records or record from sensors?
	if(argc > 2) { // record
		// if map does not exist, create a new one
		if(!map)
			// for simulation with no texture mapping from camera
			map = new Map(argv[1], 1600, 800, 0, 0.1);
			// for real-world with camera texture mapping
			//map = new Map(argv[1], 512, 512, 16, 0.05);

		// initialize world
		World world(map);

		// for simulation with no texture mapping from camera
		Robot robot(&world, argv[2],  argc > 3 ? atoi(argv[3]) : 6665);
		// for real-world with camera texture mapping
		//Robot robot(&world, argv[2],  argc > 3 ? atoi(argv[3]) : 6665, 0);

		// UI
		GLWidget gl(&world, &robot);
		gl.show();

		return a.exec();

	} else { // playback
		if(!map) {
			fprintf(stderr, "SLAM: failed to read map file: %s\n", argv[1]);
			return -2;
		}
		// initialize world
		World world(map);

		// UI
		GLWidget gl(&world);
		gl.show();
		return a.exec();
	}
}

