/*
 * Copyright (C) 2009 Ziyan Zhou <zhou@ziyan.info>
 * Visit http://ziyan.info/tag/head-tracking/ for more information
 *
 * This file is part of Head Tracking Display.
 *
 * Head Tracking Display is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Head Tracking Display is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Head Tracking Display.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libplayerc++/playerc++.h>
#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <math.h>
#include "display.h"

using namespace std;
using namespace PlayerCc;

void *player(void* arg) {
	PlayerClient client("htdwalle.rit.edu", 6666);
	Position3dProxy p3d(&client, 0);
	client.Read();
	double x, y, z;
	for(;;) {
		client.Read();
		x = p3d.GetXPos(); y = p3d.GetYPos(); z = p3d.GetZPos();
		printf("%f, %f, %f\n", x, y, z);
		display_update(x, y, z);
	}
}

int main(int argc, char **argv) {
	srand((unsigned int)time(NULL));

	if (argc < 1) {
		fprintf(stderr, "Usage: %s <hostname> [port]\n", argv[0]);
		return -1;
	}
	pthread_t thread;
	pthread_create(&thread, 0, player, 0);


	display_init(argc, argv);
	display_run();

	return 0;
}



