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

#include "motion.h"
#include "../common/camera.h"
#include <highgui.h>
#include <unistd.h>

motion::motion(ConfigFile* cf, int section) : Driver(cf, section) {
	show = string(cf->ReadString(section, "show", ""));
	PLAYER_MSG1(8,"motion: config: show = %s", show.c_str());
	if (cf->ReadDeviceAddr(&blobfinder_addr, section, "provides", PLAYER_BLOBFINDER_CODE, -1, NULL) != 0) {
		PLAYER_ERROR("motion: cannot find blobfinder addr");
		this->SetError(-1);
		return;
	}
	if (this->AddInterface(blobfinder_addr)) {
		PLAYER_ERROR("motion: cannot add blobfinder interface");
		this->SetError(-1);
		return;
	}

	if (cf->ReadDeviceAddr(&camera_addr, section, "requires", PLAYER_CAMERA_CODE, -1, NULL) != 0) {
		PLAYER_ERROR("motion: cannot find camera addr");
		this->SetError(-1);
		return;
	}
	// some default setting
	bgParams.win_size = 100;
	bgParams.n_gauss = 10;
	bgParams.bg_threshold = 0.25;
	bgParams.std_threshold = 4.5;
	bgParams.minArea = 200;
	bgParams.weight_init = 0.05;
	bgParams.variance_init = 30;
	PLAYER_MSG0(8,"motion: initialized");
}

motion::~motion() {
}

int motion::Setup() {
	if(show!="") cvNamedWindow(show.c_str(), 1);
	if(!(camera_dev = deviceTable->GetDevice(camera_addr))) {
		PLAYER_ERROR("motion: unable to locate suitable camera device");
		return -1;
	}
	if(camera_dev->Subscribe(InQueue) != 0) {
		PLAYER_ERROR("motion: unable to subscribe to camera device");
		return -1;
	}
	frame = 0;
	bgModel = 0;
	storage = cvCreateMemStorage(0);
	PLAYER_MSG0(8,"motion: setup");
	this->StartThread();
	return 0;
}

int motion::Shutdown() {
	PLAYER_MSG0(8,"motion: shutting down");
	this->StopThread();
	camera_dev->Unsubscribe(this->InQueue);	
	cvReleaseImage(&frame);
	cvReleaseMemStorage(&storage);
	if(show!="")
		cvDestroyWindow(show.c_str());
	PLAYER_MSG0(8,"motion: shutted down");
	return 0;
}

int motion::ProcessMessage(QueuePointer &resp_queue, player_msghdr *hdr, void *data) {
	if(Message::MatchMessage(hdr, PLAYER_MSGTYPE_DATA, PLAYER_CAMERA_DATA_STATE, camera_addr)) {
		int status = camera_retrieve((player_camera_data_t*)data, &frame);
		if(status == -3) {
			PLAYER_ERROR("motion: compressed image unsupported");
			return 0;
		} else if(status == -2) {
			PLAYER_ERROR("motion: image format incorrect");
			return 0;
		} else if(status != 0) {
			PLAYER_ERROR("motion: unknown error");
			return 0;
		}

		cvClearMemStorage(storage);

		static player_blobfinder_data_t data;
		data.width = frame->width;
		data.height = frame->height;
		data.blobs_count = 0;
		data.blobs = 0;

		if(!bgModel)
			bgModel = cvCreateGaussianBGModel(frame, &bgParams);
		else
			cvUpdateBGStatModel(frame, bgModel);

		CvSeq *seq = 0;
		cvFindContours(bgModel->foreground, storage, &seq, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		CvBox2D max;
		max.center.x = 0; max.center.y = 0;
		max.size.width = 0; max.size.height = 0;
		max.angle = 0.0;

		for( ; seq != 0; seq = seq->h_next )
		{
			CvBox2D box = cvMinAreaRect2(seq, storage);
			if(box.size.width*box.size.height > max.size.width * max.size.height)
				max = box;
		}

		if(max.size.width*max.size.height > 0) {
			static player_blobfinder_blob_t blob;
			data.blobs_count = 1;
			data.blobs = &blob;

			blob.x = max.center.x;
			blob.y = max.center.y;
			blob.area = max.size.width * max.size.height;
			blob.color = rand() % 0xFFFFFF;

			CvPoint2D32f pt[4];
			cvBoxPoints(max, pt);
			int left, right, top, bottom;
			left = cvRound(pt[0].x);
			right = cvRound(pt[0].x);
			top = cvRound(pt[0].y);
			bottom = cvRound(pt[0].y);
			for(int i=0; i<4; i++) {
				int x = cvRound(pt[i].x);
				int y = cvRound(pt[i].y);
				if(x < left) left = x;
				if(x > right) right = x;
				if(y < top) top = y;
				if(y > bottom) bottom = y;
			}
			blob.left = left < 0 ? 0 : left;
			blob.right = right >= (int)data.width ? data.width - 1 : right;
			blob.top = top < 0 ? 0 : top;
			blob.bottom = bottom >= (int)data.height ? data.height - 1 : bottom;
			if(blob.x < 0) blob.x = 0;
			if(blob.x >= data.width) blob.x = data.width - 1;
			if(blob.y < 0) blob.y = 0;
			if(blob.y >= data.height) blob.y = data.height - 1;

			PLAYER_MSG4(8, "motion: bounding box (%i, %i, %i, %i)", blob.top, blob.left, blob.bottom, blob.right);			
			if(show!="") {
				CvScalar color = CV_RGB(rand()%256, rand()%256, rand()%256);
				cvLine(frame, cvPoint((int)pt[0].x, (int)pt[0].y), cvPoint((int)pt[1].x, (int)pt[1].y), color, 1, CV_AA);
				cvLine(frame, cvPoint((int)pt[1].x, (int)pt[1].y), cvPoint((int)pt[2].x, (int)pt[2].y), color, 1, CV_AA);
				cvLine(frame, cvPoint((int)pt[2].x, (int)pt[2].y), cvPoint((int)pt[3].x, (int)pt[3].y), color, 1, CV_AA);
				cvLine(frame, cvPoint((int)pt[3].x, (int)pt[3].y), cvPoint((int)pt[0].x, (int)pt[0].y), color, 1, CV_AA);
			}
		}
		this->Publish(blobfinder_addr, PLAYER_MSGTYPE_DATA, PLAYER_BLOBFINDER_DATA_BLOBS, &data);

		if(show!="") {
			cvShowImage(show.c_str(), frame);
			cvWaitKey(10);
		}
		return 0;
	}
	return -1;
}

void motion::Main() {
	PLAYER_MSG0(8,"motion: thread started");
	for(;;) {
		pthread_testcancel();
		this->ProcessMessages();
		usleep(25000);
	}
}

Driver* motion_Init(ConfigFile *cf, int section) {
	PLAYER_MSG0(8,"motion: initializing");
	return new motion(cf, section);
}

void motion_Register(DriverTable* table) {
	table->AddDriver("motion", motion_Init);
}

extern "C" {
	int player_driver_init(DriverTable *table) {
		motion_Register(table);
		return 0;
	}
}


