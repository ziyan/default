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

#include "face.h"
#include "../common/camera.h"
#include <highgui.h>
#include <unistd.h>

face::face(ConfigFile* cf, int section) : Driver(cf, section) {
	show = string(cf->ReadString(section, "show", ""));
	cascade = (CvHaarClassifierCascade*)cvLoad(cf->ReadString(section, "cascade", "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt2.xml"), 0, 0, 0 );
	PLAYER_MSG1(8,"face: config: show = %s", show.c_str());
	if (cf->ReadDeviceAddr(&blobfinder_addr, section, "provides", PLAYER_BLOBFINDER_CODE, -1, NULL) != 0) {
		PLAYER_ERROR("face: cannot find blobfinder addr");
		this->SetError(-1);
		return;
	}
	if (this->AddInterface(blobfinder_addr)) {
		PLAYER_ERROR("face: cannot add blobfinder interface");
		this->SetError(-1);
		return;
	}

	if (cf->ReadDeviceAddr(&camera_addr, section, "requires", PLAYER_CAMERA_CODE, -1, NULL) != 0) {
		PLAYER_ERROR("face: cannot find camera addr");
		this->SetError(-1);
		return;
	}
	PLAYER_MSG0(8,"face: initialized");
}

face::~face() {
}

int face::Setup() {
	if(show!="") cvNamedWindow(show.c_str(), 1);
	if(!(camera_dev = deviceTable->GetDevice(camera_addr))) {
		PLAYER_ERROR("face: unable to locate suitable camera device");
		return -1;
	}
	if(camera_dev->Subscribe(InQueue) != 0) {
		PLAYER_ERROR("face: unable to subscribe to camera device");
		return -1;
	}
	frame = 0;
	storage = cvCreateMemStorage(0);
	PLAYER_MSG0(8,"face: setup");
	this->StartThread();
	return 0;
}

int face::Shutdown() {
	PLAYER_MSG0(8,"face: shutting down");
	this->StopThread();
	camera_dev->Unsubscribe(this->InQueue);	
	cvReleaseImage(&frame);
	cvReleaseMemStorage(&storage);
	if(show!="")
		cvDestroyWindow(show.c_str());
	PLAYER_MSG0(8,"face: shutted down");
	return 0;
}

int face::ProcessMessage(QueuePointer &resp_queue, player_msghdr *hdr, void *data) {
	if(Message::MatchMessage(hdr, PLAYER_MSGTYPE_DATA, PLAYER_CAMERA_DATA_STATE, camera_addr)) {
		int status = camera_retrieve((player_camera_data_t*)data, &frame);
		if(status == -3) {
			PLAYER_ERROR("face: compressed image unsupported");
			return 0;
		} else if(status == -2) {
			PLAYER_ERROR("face: image format incorrect");
			return 0;
		} else if(status != 0) {
			PLAYER_ERROR("face: unknown error");
			return 0;
		}

		static player_blobfinder_data_t data;
		data.width = frame->width;
		data.height = frame->height;
		data.blobs_count = 0;
		data.blobs = 0;

		cvClearMemStorage(storage);

		CvSeq* faces = cvHaarDetectObjects(frame, cascade, storage, 1.2, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(20,20));

		CvRect *max = 0;
		for(int i = 0; i< (faces?faces->total:0); i++)
		{
			CvRect* f = (CvRect*)cvGetSeqElem(faces, i);
			if(!max) max = f;
			else if(f->width*f->height>max->width*max->height)
				max = f;
		}

		if(max) {
			static player_blobfinder_blob_t blob;
			data.blobs_count = 1;
			data.blobs = &blob;

			blob.x = max->x + max->width / 2;
			blob.y = max->y + max->height / 2;
			blob.area = max->width*max->height;
			blob.left = max->x;
			blob.top = max->y;
			blob.right = max->x + max->width;
			blob.bottom = max->y + max->height;
			blob.color = rand() % 0xFFFFFF;

			PLAYER_MSG2(8, "face: found face at (%i, %i)", blob.x, blob.y);

			if(show!="")
				cvRectangle(frame, cvPoint(max->x, max->y), cvPoint(max->x+max->width, max->y+max->height), CV_RGB(255,0,0), 3, 8, 0);
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

void face::Main() {
	PLAYER_MSG0(8,"face: thread started");
	for(;;) {
		pthread_testcancel();
		this->ProcessMessages();
		usleep(25000);
	}
}

Driver* face_Init(ConfigFile *cf, int section) {
	PLAYER_MSG0(8,"face: initializing");
	return new face(cf, section);
}

void face_Register(DriverTable* table) {
	table->AddDriver("face", face_Init);
}

extern "C" {
	int player_driver_init(DriverTable *table) {
		face_Register(table);
		return 0;
	}
}


