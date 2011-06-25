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

#define PLAYER
#ifdef PLAYER
#include "../common/camera.h"
#include <libplayerc++/playerc++.h>
#endif

#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <cv.h>
#include <highgui.h>
#include <cvaux.h>

using namespace std;

#ifdef PLAYER
using namespace PlayerCc;
#endif

bool killed = false;
void kill(int i = 0) {
	killed = true;
}

int main(const int argc, const char** argv) {
	signal(SIGINT, kill);
	signal(SIGTERM, kill);
	signal(SIGABRT, kill);

	#ifdef PLAYER
	if(argc<2) {
		fprintf(stderr,"Usage: %s <hostname> [port] [cam-id]\n", argv[0]);
		return -1;
	}

	PlayerClient robot(argv[1], argc>2?atoi(argv[2]):6665);
	CameraProxy camera(&robot, argc>3?atoi(argv[3]):0);
	#else
	CvCapture *capture = cvCaptureFromCAM(0);
	#endif
	IplImage *frame = 0;
	IplImage *src = 0;
	//IplImage *h_plane = 0, *s_plane = 0, *v_plane = 0, *v_plane2 = 0;
	//IplImage *dst = 0;
	//IplImage *hsv = 0;

	cvNamedWindow("Source", 1);
	//cvNamedWindow("H", 1);
	//cvNamedWindow("S", 1);
	//cvNamedWindow("After", 1);
	cvNamedWindow("BG", 1);
	cvNamedWindow("FG", 1);

	CvGaussBGStatModelParams params;
	params.win_size = 100;
	params.n_gauss = 10;
	params.bg_threshold = 0.3;
	params.std_threshold = 3.5;
	params.minArea = 500;
	params.weight_init = 0.05;
	params.variance_init = 30;
	CvBGStatModel* bgModel = 0;

	CvMemStorage *storage = cvCreateMemStorage(0);

	while(!killed) {
		#ifdef PLAYER
		robot.Read();
		camera_retrieve(&camera, &frame);
		#else
		cvGrabFrame(capture);
		frame = cvRetrieveFrame(capture);
		#endif
		if(!frame) continue;
		//if(!src) src = cvCreateImage(cvSize(cvRound(frame->width/2.0),cvRound(frame->height/2.0)), 8, 3);
		//cvResize(frame, src, CV_INTER_LINEAR);
		src = frame;
		/*if(!hsv) hsv = cvCreateImage(cvGetSize(src), 8, 3);
		cvCvtColor(src, hsv, CV_BGR2HSV);
		if(!h_plane) h_plane = cvCreateImage(cvGetSize(src), 8, 1);
		if(!s_plane) s_plane = cvCreateImage(cvGetSize(src), 8, 1);
		if(!v_plane) v_plane = cvCreateImage(cvGetSize(src), 8, 1);
		if(!v_plane2) {
			v_plane2 = cvCreateImage(cvGetSize(src), 8, 1);
			unsigned char *v = (unsigned char*)v_plane2->imageData;
			for(int i = 0; i< v_plane2->width*v_plane2->height; i++) *(v++) = 200;
		}
		cvCvtPixToPlane(hsv, h_plane, s_plane, v_plane, 0);
		//unsigned char *v = (unsigned char*)v_plane->imageData;
		//for(int i = 0; i< v_plane->width*v_plane->height; i++) *(v++) = 255;
		cvCvtPlaneToPix(h_plane, s_plane, v_plane2, 0, hsv);

		//if(!dst) dst = cvCreateImage(cvGetSize(src), 8, 3);
		cvCvtColor(hsv, hsv, CV_HSV2BGR);
		dst = hsv;*/
		//cvCvtColor(hsv, dst, CV_BGR2GRAY);

		if(!bgModel) bgModel = cvCreateGaussianBGModel(src, &params);
		else cvUpdateBGStatModel(src, bgModel);

		CvSeq *seq = 0;
		cvFindContours(bgModel->foreground, storage, &seq, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		for( ; seq != 0; seq = seq->h_next )
		{
			CvBox2D box = cvMinAreaRect2(seq, storage);
			CvPoint2D32f pt[4];
			cvBoxPoints(box, pt);
			CvScalar color = CV_RGB( rand()%256, rand()%256, rand()%256 );
			cvLine(src, cvPoint( (int)pt[0].x, (int)pt[0].y), cvPoint( (int)pt[1].x, (int)pt[1].y), color, 1, CV_AA);
			cvLine(src, cvPoint( (int)pt[1].x, (int)pt[1].y), cvPoint( (int)pt[2].x, (int)pt[2].y), color, 1, CV_AA);
			cvLine(src, cvPoint( (int)pt[2].x, (int)pt[2].y), cvPoint( (int)pt[3].x, (int)pt[3].y), color, 1, CV_AA);
			cvLine(src, cvPoint( (int)pt[3].x, (int)pt[3].y), cvPoint( (int)pt[0].x, (int)pt[0].y), color, 1, CV_AA);
			cvDrawContours( src, seq, color, color, -1, CV_FILLED, 8 );
		}

		cvShowImage("BG", bgModel->background);
		cvShowImage("FG", bgModel->foreground);

		cvShowImage("Source", src);
		//cvShowImage("H", h_plane);
		//cvShowImage("S", s_plane);
		//cvShowImage("After", hsv);

		if(cvWaitKey(10) == 27) break;
	}
	cvReleaseImage(&frame);
	//cvReleaseImage(&src);
	//cvReleaseImage(&h_plane);
	//cvReleaseImage(&s_plane);
	//cvReleaseImage(&v_plane);
	//cvReleaseImage(&v_plane2);
	//cvReleaseImage(&hsv);
	//cvReleaseImage(&dst);
}

