// Copyright (C) Ziyan Zhou <zhou@ziyan.info> All right reserved.

// OpenGL includes
#include <GL/glut.h>
#include "glm.h"
// C includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
// OpenCV includes
#include <opencv/cv.h>
#include <opencv/highgui.h>
// pthread
#include <pthread.h>

typedef struct _Eye {
	double h, v, r;
} Eye;

typedef struct _Laser {
	double speed, x, y, z, length;
} Laser;

/* Variables */
const double cam_wide = M_PI / 1.5;
const double cam_dist = 0.2;
const double cam_aspect = 3.0 / 4.0;
const double frame_rate = 1000.0 / 35.0;
//OpenGL
int window;
int state = 0; //0 = fullscreen, 1 = window, -1 = exit
int width, height; //remember windows size
Eye eye, cam;
int tw, th;
unsigned char* image;

#define WINDOW_HEIGHT 1.0
#define RAND_DOUBLE ((double)rand()/(double)RAND_MAX)
#define TARGET_COUNT 7
#define FINENESS (M_PI / 180.0)

//pthread
pthread_t track_thread;
double targets[TARGET_COUNT*3];


void grid(double width, double height, double depth, double space) {
	glLineWidth(2);
	double x,y,z;
	for(x=-width/2;x<=width/2;x+=space)
	{
		glBegin(GL_LINES);
			glVertex3f(0.0,x,-height/2);
			glVertex3f(-depth/2,x,-height/2);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(0.0,x,height/2);
			glVertex3f(-depth/2,x,height/2);
		glEnd();
	}
	for(y=-height/2;y<=height/2;y+=space)
	{
		/*glBegin(GL_LINES);
			glVertex3f(-width/2,y,-depth/2);
			glVertex3f(width/2,y,-depth/2);
		glEnd();*/
		glBegin(GL_LINES);
			glVertex3f(0.0,-width/2,y);
			glVertex3f(-depth/2,-width/2,y);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(0.0,width/2,y);
			glVertex3f(-depth/2,width/2,y);
		glEnd();
	}
	for(z=-depth/2;z<=0.0;z+=space)
	{
		glBegin(GL_LINES);
			glVertex3f(z,-width/2,height/2);
			glVertex3f(z,width/2,height/2);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(z,-width/2,-height/2);
			glVertex3f(z,width/2,-height/2);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(z,width/2,-height/2);
			glVertex3f(z,width/2,height/2);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(z,-width/2,-height/2);
			glVertex3f(z,-width/2,height/2);
		glEnd();
	}
}


void target(double x, double y, double z, double r) {
	glLineWidth(5);
	glLineStipple(1,0xFF00);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex3f(-40,0,0);
		glVertex3f(x,y,z);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	glPushMatrix();
	glTranslatef(x,y,z);
	glEnable(GL_TEXTURE_2D);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tw, th, GL_RGB, GL_UNSIGNED_BYTE, image);

	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5, 0.5);
	glVertex3f(0.0, 0.0, 0.0);
	double a;
	for(a = -M_PI; a < M_PI; a += FINENESS) {
		glTexCoord2f(cos(a)/2.0+0.5, sin(a)/2.0+0.5);
		glVertex3f(0.0, cos(a)*r, sin(a)*r);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void draw()
{
	//grid
	glColor4f(1,1,1,0.5);
	grid(2.0*(double)width/(double)height*WINDOW_HEIGHT,2.0*WINDOW_HEIGHT,40,0.33);
	//targets
	int i;
	for(i = 0; i < TARGET_COUNT; i++)
		target(targets[i*3+0], targets[i*3+1], targets[i*3+2], 0.33);
	target(1.0, -0.2, 0.3, 0.33);
}

void init() 
{
	// set background
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// anti-alias
	glShadeModel(GL_SMOOTH);

	// lighting
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 30.0, 30.0, 30.0, 30.0 };
	GLfloat mat_surface[] = { 0.075, 0.075, 0.075 };
	GLfloat mat_diffuse[] = { 0.25, 0.25, 0.25, 0.25 };
	
	GLfloat ambient_light[] = { 0.4, 0.4, 0.4};
	GLfloat white_light[] = { 1.0, 1.0,1.0, 0.4};
	GLfloat light_position0[] = { -30.0, 30.0, 15.0 };
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_surface);
	
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT0, GL_EMISSION, white_light);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);


	glEnable(GL_DEPTH_TEST);


	// fog
	glEnable(GL_FOG);

	GLfloat fog_color[] = {0,0,0,1.0};
	glFogfv(GL_FOG_COLOR, fog_color);
	glFogi(GL_FOG_MODE, GL_LINEAR);

	// texture
	image = glmReadPPM("data/target.ppm", &tw, &th);

	// random target
	int i;
	for(i = 0; i < TARGET_COUNT; i++) {
		targets[i*3+0] = -(RAND_DOUBLE * 8.0);
		targets[i*3+1] = RAND_DOUBLE * 2.5 - 1.25;
		targets[i*3+2] = RAND_DOUBLE * 1.5 - 0.75;
	}
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	double x = cos(cam.v) * cos(cam.h) * cam.r;
	double y = sin(cam.h) * cos(cam.v) * cam.r;
	double z = sin(cam.v) * cam.r;
	double vfov = (atan((WINDOW_HEIGHT-z)/sqrt(x*x+y*y)) + atan((WINDOW_HEIGHT+z)/sqrt(x*x+y*y)))/2.0;
	double lz = z - tan(vfov - atan((WINDOW_HEIGHT - z)/sqrt(x*x+y*y))) * sqrt(x*x+y*y);
	double ratio = (double)width / (double)height;
	double hfov = (atan((WINDOW_HEIGHT*ratio-y)/sqrt(x*x+z*z)) + atan((WINDOW_HEIGHT*ratio+y)/sqrt(x*x+z*z)))/2.0;
	double ly = y - tan(hfov - atan((WINDOW_HEIGHT*ratio - y)/sqrt(x*x+z*z))) * sqrt(x*x+z*z);

	gluLookAt( x, y, z, 0.0, ly, lz, 0.0, 0.0, 1.0);
	double r = sqrt(x*x+y*y+z*z);
		glEnable(GL_FOG);
		glFogf(GL_FOG_START, 5 + cam.r);
		glFogf(GL_FOG_END, 10 + cam.r);
	draw();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	width = w;
	height = h;
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double x = cos(cam.v) * cos(cam.h) * cam.r;
	double y = sin(cam.h) * cos(cam.v) * cam.r;
	double z = sin(cam.v) * cam.r;
	double vfov = (atan((WINDOW_HEIGHT-z)/sqrt(x*x + y*y)) + atan((WINDOW_HEIGHT+z)/sqrt(x*x + y*y))) / M_PI * 180.0;
	double ratio = (double)width / (double)height;
	double hfov = (atan((WINDOW_HEIGHT*ratio-y)/sqrt(x*x + z*z)) + atan((WINDOW_HEIGHT*ratio+y)/sqrt(x*x + z*z))) / M_PI * 180.0;
	gluPerspective(vfov, hfov/vfov, 0.001, 200.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void redisplay(void)
{
	glutSetWindow(window);
	reshape(width,height);
	glutPostRedisplay();
}

void *track(void *arg) {
	//OpenCV
	CvMemStorage* storage = 0;
	CvHaarClassifierCascade* cascade = 0;
	CvCapture* capture = 0;
	IplImage *frame = 0;
	IplImage *image = 0;
	CvScalar color = {{255,255,255}};
	double scale = 3.0;
	double h = 0,v = 0,r = 0,t = 0;

	//setup opencv
	cascade = (CvHaarClassifierCascade*)cvLoad("data/haarcascade_frontalface_alt2.xml", 0, 0, 0 );
	storage = cvCreateMemStorage(0);
	capture = cvCaptureFromCAM(0);
	cvNamedWindow("OpenCV",1);

	while(state>=0) {
		//grab the frame
		if(!cvGrabFrame(capture)) return;
		frame = cvRetrieveFrame(capture);
		if(!frame) return;
		if(!image) image=cvCreateImage(cvSize(frame->width,frame->height),IPL_DEPTH_8U,frame->nChannels);
		if(frame->origin == IPL_ORIGIN_TL)
			cvCopy(frame,image,0);
		else
			cvFlip(frame,image,0);
	
		//convert to gray
		IplImage* gray = cvCreateImage(cvSize(image->width,image->height),8,1);
		cvCvtColor(image,gray,CV_BGR2GRAY);
		
		//downsize the image
		IplImage* small_img = cvCreateImage(cvSize(cvRound(gray->width/scale),cvRound(gray->height/scale)),8,1);
		cvResize(gray,small_img,CV_INTER_LINEAR);
		cvEqualizeHist(small_img,small_img);

		//clear storage
		cvClearMemStorage(storage);
		//detect face
		
		CvSeq* faces=cvHaarDetectObjects(small_img,cascade,storage,1.1,2,CV_HAAR_DO_CANNY_PRUNING,cvSize(20,20));
		
		int i,area_max = 0;
		for(i=0;i<(faces?faces->total:0);i++)
		{
			CvRect* f=(CvRect*)cvGetSeqElem(faces,i);
			//find max
			if(f->width*f->height>area_max)
			{
				area_max = f->width*f->height;
				//calculate eye position
				r = (image->width*image->height) / (f->width*f->height*scale*scale) * cam_dist;
				h = -((f->x+f->width*0.5)*scale/image->width*2.0-1.0) * cam_wide * 0.5;
				v = -((f->y+f->height*0.5)*scale/image->height*2.0-1.0) * cam_wide * 0.5 * cam_aspect;
			}
			if(state == 1)
				cvRectangle(gray,cvPoint(f->x*scale, f->y*scale), cvPoint((f->x+f->width)*scale, (f->y+f->height)*scale), color, 3, 8, 0);
		}
		Eye new_eye = eye;

		new_eye.h= h;
		new_eye.v= v;
		new_eye.r= r;
		
		if(state == 1)
			cvShowImage("OpenCV",gray);
		//relase resource
		cvReleaseImage(&gray);
		cvReleaseImage(&small_img);
		//show the picture
		cvWaitKey(25);
		if(state == 1)
			printf("Eye = %f, %f, %f\n", new_eye.h, new_eye.v, new_eye.r);
		eye = new_eye;
	}
	//free up resource
	cvReleaseImage( &image );
        cvReleaseCapture( &capture );
}

void idle(void) {
	usleep((int)(frame_rate * 1000.0));
	cam.h= cam.h * (1.0 - 0.1) + 0.1 * eye.h;
	cam.v= cam.v * (1.0 - 0.1) + 0.1 * eye.v;
	cam.r= cam.r * (1.0 - 0.1) + 0.1 * eye.r;
	redisplay();
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 27:
			exit(0);
			break;
		case 'c':
			if(state == 0) {
				glutLeaveGameMode();
				glutInitWindowSize(1024, 768); 
				glutInitWindowPosition(0, 0);
				window = glutCreateWindow("Head Tracking Display");
				glutSetCursor(GLUT_CURSOR_INHERIT);
				state = 1;
			} else {
				glutDestroyWindow(window);
				window = glutEnterGameMode();
				glutSetCursor(GLUT_CURSOR_NONE);
				state = 0;
			}
			init();
			glutDisplayFunc(display); 
			glutReshapeFunc(reshape);
			glutKeyboardFunc(keyboard);
			glutIdleFunc(idle);
			break;
	}
}

int main(int argc, char** argv)
{
	
	//srand((unsigned int)time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutGameModeString( "1440x900:32@60" );
	window = glutEnterGameMode();
	glutSetCursor(GLUT_CURSOR_NONE);

	//glutInitWindowSize(1024, 768); 
	//glutInitWindowPosition(0, 0);

	
	//window = glutCreateWindow("Head Tracking Display");
	init();
	
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	//setup thread
	pthread_create(&track_thread, NULL, &track, NULL);

	glutMainLoop();

	//terminate thread
	state = -1;
	pthread_join(track_thread, NULL);
}

