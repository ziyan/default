/**
 * Vision Module for IGVC Competition Bot
 */

#include "Camera.h"
#include "CameraData.h"
#include "CameraFrame.h"

#include <cmath>
#include <sstream>

#define BILATERAL_COLOR_SIGMA 50
#define BILATERAL_SPACE_SIGMA 3
#define MEDIAN_KERNEL 7
#define GAUSSIAN_KERNEL 7

using namespace erc;
using namespace std;

Camera::Camera(DebugStream& debug, libconfig::Setting &setting) :
	LocalModule(debug,setting),
	good(false),
	gui(false),
	process(false),
	storage(0),
	capture(0),
	currentData(0),
	currentFrame(0),
	camid(0),
	BILATERAL_ITER(2),
	CANNY_LOW(60),
	CANNY_HIGH(100),
	HOUGH_THRESHOLD(50)
{
	camid = (int)setting["camera"];
	process = (bool)setting["process"];
	gui = (bool)setting["gui"];
	HOUGH_THRESHOLD = (int)setting["hough_threshold"];
	CANNY_LOW = (int)setting["canny_low"];
	CANNY_HIGH = (int)setting["canny_high"];
	BILATERAL_ITER = (int)setting["bilateral_iter"];
	storage = cvCreateMemStorage(0);
	capture = cvCaptureFromCAM(camid);
	good = (capture);
}

Camera::~Camera()
{
}

bool Camera::isGood() const
{
	return good;
}

void Camera::run()
{
	stringstream ss;
	ss<<"Camera "<<camid<<": ";
	ss.flush();
	string window_name_prefix = ss.str();

	//creating windows
	if(gui)
	{
		
		cvNamedWindow((window_name_prefix+"Raw").c_str(), CV_WINDOW_AUTOSIZE);
		cvNamedWindow((window_name_prefix+"Hough").c_str(), CV_WINDOW_AUTOSIZE);
		cvNamedWindow((window_name_prefix+"BW").c_str(), CV_WINDOW_AUTOSIZE);
		cvNamedWindow((window_name_prefix+"Control").c_str(), CV_WINDOW_AUTOSIZE);
		cvCreateTrackbar( "H T", (window_name_prefix+"Control").c_str(), &HOUGH_THRESHOLD, 100, 0 );
		cvCreateTrackbar( "C L T", (window_name_prefix+"Control").c_str(), &CANNY_LOW, 1000, 0 );
		cvCreateTrackbar( "C H T", (window_name_prefix+"Control").c_str(), &CANNY_HIGH, 1000 , 0);
		cvCreateTrackbar( "B I", (window_name_prefix+"Control").c_str(), &BILATERAL_ITER, 10, 0);
	}

	while(isGood())
	{
		//grab a new frame
		if(!cvGrabFrame( capture )) break;
		IplImage *frame = cvRetrieveFrame( capture );
		if( !frame || !isGood()) break;

		//put frame data into cache
		mutex.enterMutex();
		CameraData *data = new CameraData;
		data->width = frame->width;
		data->height = frame->height;
		data->state = CAMERA_NO_LINES;

		CameraFrame *img = new CameraFrame;
		img->width = frame->width;
		img->height = frame->height;
		img->channel = frame->nChannels;
		img->frame = new unsigned char[img->width*img->height*img->channel];
		copy(frame->imageData,frame->imageData+(img->width*img->height*img->channel),img->frame);
		currentFrame = DataPointer(img);
		currentData = DataPointer(data);
		mutex.leaveMutex();


		if( !isGood() ) break;

		if(process)
		{
			//get lines
			mutex.enterMutex();

			IplImage *image = cvCreateImage(cvGetSize(frame),8,3);
			IplImage *bw = cvCreateImage(cvGetSize(frame),8,1);
			image->origin = frame->origin;
			IplImage *dst = cvCreateImage( cvGetSize(frame),8,1);
			//IplImage *binary = cvCreateImage(cvGetSize(frame),8,1);

      			Line left, right, center;
			//LineState state = NO_MIRROR;
			bool left_bad = false, right_bad = false;

			//Get working copy
			cvCopy(frame, image);

			//Make several passes with bilateral filter
			if(BILATERAL_ITER<0) BILATERAL_ITER = 0;
			if(BILATERAL_ITER>10) BILATERAL_ITER = 10;
			for(short int i = 0; i < BILATERAL_ITER; i++)
			{
				cvSmooth( image,image,CV_BILATERAL,BILATERAL_COLOR_SIGMA,BILATERAL_SPACE_SIGMA,0,0);
			}

			//Create grayscale copy
			cvCvtColor(image,bw,CV_RGB2GRAY);

			//Cleanup Grayscale
			cvEqualizeHist(bw,bw); //Going to replaced by handwritten function. see note in vision.c

			//Extract edges
			cvCanny( bw,dst,CANNY_LOW,CANNY_HIGH,3);

			//Left Side
			cvSetImageROI(dst,cvRect(0,0,dst->width/2,dst->height));

			//get the lines
			left_bad = !processSide(dst, left);

			//Right Side
			cvSetImageROI(dst,cvRect(dst->width/2,0,dst->width,dst->height));

			//get the lines
			right_bad = !processSide(dst, right);
				
			//shift it back to right
			right.p1.x += dst->width/2;
			right.p2.x += dst->width/2;

			//do the mirror
			if(left_bad && right_bad)
			{
				//We'd better panic, we can't see where we're going
				data->state = CAMERA_NO_LINES;
			}
			else if(left_bad)
			{
				left.p1.x = dst->width - right.p1.x;
				left.p1.y = right.p1.y;
				left.p2.x = dst->width - right.p2.x;
				left.p2.y = right.p2.y;
				data->state = CAMERA_LEFT_MIRROR;
			}
			else if(right_bad)
			{
				right.p1.x = dst->width - left.p1.x;
				right.p1.y = left.p1.y;
				right.p2.x = dst->width - left.p2.x;
				right.p2.y = left.p2.y;
				data->state = CAMERA_RIGHT_MIRROR;
			}
			else
			{
				data->state = CAMERA_NO_MIRROR; //Everything is good.
			}

			//Calculate center line
			if(!(left_bad&&right_bad))
			{
				double lk = 0, rk = 0, ck = 0;
				short int lb = 0, rb = 0, cb = 0;
				
				//Left k && y intercept
				if(left.p1.x - left.p2.x!=0)
				{
					lk = (double)(left.p1.y - left.p2.y) / (double)(left.p1.x - left.p2.x);
					lb = (short int)(lk * (-left.p1.x)) + left.p1.y;
					
					left.p1.x = 0;
					left.p2.x = dst->width;
					left.p1.y = lb;
					left.p2.y = (short int)(lk * (dst->width) + lb);

					if(left.p1.y<0 && lk!=0)
					{
						left.p1.y = 0;
						left.p1.x = (short int)((double)(left.p1.y - lb)/lk);
					}
					else if(left.p1.y > dst->height && lk!=0)
					{
						left.p1.y = dst->height;
						left.p1.x = (short int)((double)(left.p1.y - lb)/lk);
					}

					if(left.p2.y<0 && lk!=0)
					{
						left.p2.y = 0;
						left.p2.x = (short int)((double)(left.p2.y - lb)/lk);
					}
					else if(left.p2.y > dst->height && lk!=0)
					{
						left.p2.y = dst->height;
						left.p2.x = (short int)((double)(left.p2.y - lb)/lk);
					}
					
				}
				else
				{
					//we got a holly straight line!
					left.p1.y = 0;
					left.p2.y = dst->height;
				}
				//Right Slope & y intercept
				if(right.p1.x - right.p2.x!=0)
				{
					rk = (double)(right.p1.y - right.p2.y) / (double)(right.p1.x - right.p2.x);
					rb = (short int)(rk * (-right.p1.x)) + right.p1.y;
					
					right.p1.x = 0;
					right.p2.x = dst->width;
					right.p1.y = rb;
					right.p2.y = (short int)(rk * (dst->width) + rb);

					if(right.p1.y<0 && rk!=0)
					{
						right.p1.y = 0;
						right.p1.x = (short int)((double)(right.p1.y - rb)/rk);
					}
					else if(right.p1.y > dst->height && rk!=0)
					{
						right.p1.y = dst->height;
						right.p1.x = (short int)((double)(right.p1.y - rb)/rk);
					}

					if(right.p2.y<0 && rk!=0)
					{
						right.p2.y = 0;
						right.p2.x = (short int)((double)(right.p2.y - rb)/rk);
					}
					else if(right.p2.y > dst->height && rk!=0)
					{
						right.p2.y = dst->height;
						right.p2.x = (short int)((double)(right.p2.y - rb)/rk);
					}
					
				}
				else
				{
					//we got a holly straight line!
					right.p1.y = 0;
					right.p2.y = dst->height;
				}
				//Middle of image is first part of center line
				center.p1.x = dst->width/2;
				center.p1.y = dst->height;

				center.p2.x = (int)((double)(rb - lb) / (double)(lk - rk));
				center.p2.y = (int)(lk * center.p2.x + lb);
				
				if(center.p1.x-center.p2.x!=0)
				{
					ck = (center.p1.y - center.p2.y) / (center.p1.x - center.p2.x);
					cb = (short int)(ck * (-center.p1.x)) + center.p1.y;

					if(ck!=0)
					{
						center.p2.y = 0;
						center.p2.x = (short int)((double)(center.p2.y - cb)/ck);
					}

					if(center.p2.x < 0)
					{
						center.p2.x = 0;
						center.p2.y = cb;
					}
					else if(center.p2.x > dst->width)
					{
						center.p2.x = dst->width;
						center.p2.y = (short int)(ck * (dst->width) + cb);
					}
				}
				else
				{
					//we got a holly straight line!
					center.p1.y = dst->height;
					center.p2.y = 0;
				}
				
			}
			//Assign Data
			data->left = left;
			data->right = right;
			data->center = center;
			cvResetImageROI(dst); //Stop restricting view of world
			currentData = DataPointer(data);
			
			if(gui)
			{
			
				cvLine( image, cvPoint(left.p1.x, left.p1.y),cvPoint(left.p2.x, left.p2.y), CV_RGB(255,0,0),3, CV_AA,0);
				cvLine( image, cvPoint(right.p1.x, right.p1.y),cvPoint(right.p2.x, right.p2.y), CV_RGB(0,255,0),3, CV_AA,0);
				cvLine( image, cvPoint(center.p1.x, center.p1.y),cvPoint(center.p2.x, center.p2.y), CV_RGB(0,0,255),3, CV_AA,0);
	
				cvShowImage( (window_name_prefix+"Raw").c_str(), image);
				cvShowImage( (window_name_prefix+"Hough").c_str(), dst );
				cvShowImage( (window_name_prefix+"BW").c_str(), bw);

				cvWaitKey(10);
			}

			cvReleaseImage(&image);
			cvReleaseImage(&bw);
			cvReleaseImage(&dst);

			mutex.leaveMutex();
			//sleep a little bit
			//usleep(1000);
		}
	}
	good = false;
}


void Camera::close()
{
	good = false;
	Thread::sleep(100);
	mutex.enterMutex();
	if(gui) cvDestroyAllWindows();
	if(capture) cvReleaseCapture(&capture);
	if(storage) cvReleaseMemStorage(&storage);
	setting["hough_threshold"] = HOUGH_THRESHOLD;
	setting["canny_low"] = CANNY_LOW;
	setting["canny_high"] = CANNY_HIGH;
	setting["bilateral_iter"] = BILATERAL_ITER;
	mutex.leaveMutex();
}

DataPointer Camera::getData(const unsigned char arg) const
{
	if(arg==0)
	{
		//Return line data
		return currentData;
	}
	else
	{
		//Return entire frame
		return currentFrame;
	}
}

bool Camera::processSide(IplImage *image, Line &line)
{
	CvSeq* lines = 0;
	if(HOUGH_THRESHOLD<=0) HOUGH_THRESHOLD = 1;
	if(HOUGH_THRESHOLD>=100) HOUGH_THRESHOLD = 99;
	lines = cvHoughLines2( image, storage, CV_HOUGH_STANDARD,1,CV_PI/180,HOUGH_THRESHOLD,0,0);
	float rho = 0, theta = 0;
	for(int i = 0; i < MIN(lines->total,100); i++ )
	{
		float* line = (float*)cvGetSeqElem(lines,i);
		rho += line[0];
		theta += line[1];
		//CvPoint* points = (CvPoint*)cvGetSeqElem(lines,i);
		//cvLine( image, points[0], points[1], CV_RGB(255,255,255),3, CV_AA,0);
	}
 	if(MIN(lines->total,100) != 0)
	{
		double a,b,x0,y0;
		rho = rho/MIN(lines->total, 100);
		theta = theta/MIN(lines->total, 100);
		a=cos(theta);
		b=sin(theta);
		x0=a*rho;
		y0=b*rho;
		line.p1.x = cvRound(x0+1000*(-b));
		line.p1.y = cvRound(y0+1000*(a));
		line.p2.x = cvRound(x0-1000*(-b));
		line.p2.y = cvRound(y0-1000*(a));
		cvClearSeq(lines);
		return true;
	}
	else
	{
		cvClearSeq(lines);
		return false;
	}
}

