/**
 * A module wrapper for vision process
 */

#ifndef ERC_CAMERA_H
#define ERC_CAMERA_H

#include "../LocalModule.h"
#include "../DataPointer.h"
#include "../../common/Line.h"

#include <cv.h>
#include <highgui.h>
#include <string>

namespace erc
{
	class Camera : public LocalModule
	{
	public:
		Camera(DebugStream& debug, libconfig::Setting &setting);
		~Camera();

		bool isGood() const;
		DataPointer getData(const unsigned char arg = 0) const;
		void close();

	protected:
		void run();

	private:
		bool good;
		bool gui;
		bool process;
		int camid;

		DataPointer currentData;
		DataPointer currentFrame;

		CvMemStorage *storage;
		CvCapture* capture;
		ost::Mutex mutex;

		bool processSide(IplImage *image, Line &line);

		int BILATERAL_ITER; //2
		int CANNY_LOW; //60
		int CANNY_HIGH; //100
		int HOUGH_THRESHOLD; //50
	};
}

#endif
