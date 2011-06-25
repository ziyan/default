#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

#define DEBUG

// global killed flag
int killed = 0;

// kill signal handler
void kill_signal_handler(int i)
{
	killed = 1;
}

int main(int argc, char **argv)
{
	CvCapture *capture = 0;
	IplImage *frame = 0;
	CvVideoWriter *writer = 0;

#ifdef DEBUG
	uint32_t frame_count = 0;
	time_t t = time(NULL);
#endif

	// register kill signals
	signal(SIGINT, kill_signal_handler);
	signal(SIGTERM, kill_signal_handler);
	signal(SIGABRT, kill_signal_handler);

	// check arguments
	if (argc != 4)
	{
		fprintf(stderr, "Usage: %s <camera-id> <avi-file> <fps>\n", argv[0]);
		return -1;
	}

	// open capture
	if (!(capture = cvCaptureFromCAM(atoi(argv[1]))))
	{
		fprintf(stderr, "ERROR: failed to open device for capture\n");
		return -1;
	}
	
	if (!cvGrabFrame(capture))
	{
		fprintf(stderr, "ERROR: failed to grab frame\n");
		return -1;
	}

	if (!(frame = cvQueryFrame(capture)))
	{
		fprintf(stderr, "ERROR: failed to query\n");
		return -1;
	}

#ifdef DEBUG
	// create display window
	cvNamedWindow("debug", 1);
#endif

	// create video logger
	writer = cvCreateVideoWriter(argv[2], CV_FOURCC('M','J','P','G'), atof(argv[3]), cvSize(frame->width, frame->height), 1);

	// other possible codecs
	/*
	 * CV_FOURCC('P', 'I', 'M', '1') = MPEG-1 codec
	 * CV_FOURCC('M', 'J', 'P', 'G') = motion-jpeg codec
	 * CV_FOURCC('M', 'P', '4', '2') = MPEG-4.2 codec
	 * CV_FOURCC('D', 'I', 'V', '3') = MPEG-4.3 codec
	 * CV_FOURCC('D', 'I', 'V', 'X') = MPEG-4 codec
	 * CV_FOURCC('U', '2', '6', '3') = H263 codec
	 * CV_FOURCC('I', '2', '6', '3') = H263I codec
	 * CV_FOURCC('F', 'L', 'V', '1') = FLV1 codec
	 */

	while(!killed)
	{
		if (!cvGrabFrame(capture))
		{
			fprintf(stderr, "ERROR: failed to grab frame\n");
			break;
		}

		if (!(frame = cvQueryFrame(capture)))
		{
			fprintf(stderr, "ERROR: failed to query frame\n");
			break;
		}

		// write video frame
 		cvWriteFrame(writer, frame);

#ifdef DEBUG
		// show fps
		if (frame_count++ % 10 == 0)
			printf("cvrecord: fps = %f\n", (double)frame_count/(double)(time(NULL)-t));

		// show image
		cvShowImage("debug", frame);
		if (cvWaitKey(10) == 27)
			break;
#endif
		
	}
	cvReleaseVideoWriter(&writer);
	cvReleaseCapture(&capture);
	return(0);
}

