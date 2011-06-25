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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "../common/camera.h"
#include <libplayerc++/playerc++.h>
#include <iostream>
#include <sstream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <time.h>

using namespace std;
using namespace PlayerCc;

int main(int argc, char** argv) {
	if(argc < 2) {
		cerr << "Usage: " << argv[0] << " <hostname> [port] [id]" <<endl;
		exit(-1);
	}
	int port = argc>2?atoi(argv[2]):6665;
	PlayerClient robot(argv[1], port);
	CameraProxy camera(&robot, argc>3?atoi(argv[3]):0);
	ostringstream os;
	os<<"camviewer: "<<argv[1]<<":"<<port<<":"<<camera.GetIndex();
	string windowName = os.str();
	cvNamedWindow(windowName.c_str(),1);
	IplImage *img = 0;

	int frame_count = 0;
	time_t t = time(NULL);
	for(;;) {
		robot.Read();
		if(camera_retrieve(&camera, &img) != 0) {
			cerr << "camviewer: failed to retrieve image" << endl;
			break;
		}
		frame_count ++;
		cvShowImage(windowName.c_str(),img);
		if (cvWaitKey(10) != -1)
			break;
		if (frame_count % 20 == 0)
			cout << "camviewer: fps = " << ((double)frame_count/(double)(time(NULL)-t)) << endl;
		if (frame_count % 1000 == 0) {
			frame_count = 0;
			t = time(NULL);
		}
	}
	cvReleaseImage(&img);
	return 0;
}

