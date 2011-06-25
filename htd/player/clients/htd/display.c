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

#include "display.h"

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <GL/glut.h>

#include "../common/glm.h"
#include "texture.h"
#include "screen.h"
#include "grid.h"
#include "target.h"
#include "ball.h"

#define DISPLAY_DIST 2.0
#define DISPLAY_FOV (M_PI / 1.5)
#define WINDOW_HEIGHT 1.0
#define RAND_DOUBLE ((double)rand()/(double)RAND_MAX)
#define TARGET_COUNT 7

int window;
int state = 0; //0 = fullscreen, 1 = window, -1 = exit
int width, height; //remember windows size
double eye_x, eye_y, eye_z; // x to the front, y to the right, z to the up
double offset_x = 0.0, offset_y = 0.0, offset_z = 0.0;
int scene = 0;
double targets[TARGET_COUNT*3];

texture_t texture1, target_texture;

void init();
void draw();
void display();
void reshape(int w, int h);
void redisplay();
void keyboard(unsigned char key, int x, int y);
void idle();

void display_init(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutGameModeString("1440x900");
	window = glutEnterGameMode();
	glutSetCursor(GLUT_CURSOR_NONE);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
}

void display_update(double x, double y, double z) {
	eye_x = x;
	eye_y = y;
	eye_z = z;
}


void display_run() {
	glutMainLoop();
}

void init() {
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
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);


	glEnable(GL_DEPTH_TEST);

	// fog

	GLfloat fog_color[] = {0,0,0,1.0};
	glFogfv(GL_FOG_COLOR, fog_color);
	glFogi(GL_FOG_MODE, GL_LINEAR);

	texture1.image = glmReadPPM("data/pan.ppm", &texture1.width, &texture1.height);
	target_texture.image = glmReadPPM("data/target.ppm", &target_texture.width, &target_texture.height);


	// random target
	int i;
	for(i = 0; i < TARGET_COUNT; i++) {
		targets[i*3+0] = -(RAND_DOUBLE * 8.0);
		targets[i*3+1] = RAND_DOUBLE * 2.5 - 1.25;
		targets[i*3+2] = RAND_DOUBLE * 1.5 - 0.75;
	}
}



void scene_balls() {
	static double theta = 0.0;
	//grid
	glColor4f(1,1,1,1);
	grid(2.0*(double)width/(double)height*WINDOW_HEIGHT,2.0*WINDOW_HEIGHT,40,0.33);

	//targets
	glColor3f(0,0,1); //blue
	ball(0.0, 0.4, M_PI / 180.0 * 60.0, 0.25, theta);
	glColor3f(1,1,0); // yellow
	ball(-0.5, 0.8, M_PI / 180.0 * -30.0, 0.25, theta);
	glColor3f(1,0,0); // red
	ball(-2.0, 0.25, M_PI / 180.0 * 230.0, 0.25, theta);
	glColor3f(0,1,0); //green
	ball(1.0, 0.7, M_PI / 180.0 * -70.0, 0.25, theta);
	glColor3f(1,0,1); // purple
	ball(-1.0, 0.5, M_PI / 180.0 * 180.0, 0.25, theta);

	theta += M_PI / 180.0 / 5.0;
	while(theta >= M_PI) theta -= M_PI + M_PI;
	while(theta < -M_PI) theta += M_PI + M_PI;
}

void scene_targets() {
	//grid
	glColor4f(1,1,1,1);
	grid(2.0*(double)width/(double)height*WINDOW_HEIGHT,2.0*WINDOW_HEIGHT,40,0.33);

	//targets
	int i;
	for(i = 0; i < TARGET_COUNT; i++)
		target(targets[i*3+0], targets[i*3+1], targets[i*3+2], 0.33, &target_texture);
	target(1.0, -0.2, 0.3, 0.33, &target_texture);
}


void draw() {
	switch(scene) {
	case 0:
		scene_balls();
		break;
	case 1:
		scene_targets();
		break;
	case 2:
		glColor3f(1.0,1.0,1.0);
		screen(M_PI, (double)texture1.height/(double)texture1.width * M_PI, 15.0, 30, &texture1);
		break;
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	double x = offset_x + eye_x;
	double y = offset_y + eye_y;
	double z = offset_z + eye_z;
	double vfov = (atan((WINDOW_HEIGHT-z)/sqrt(x*x+y*y)) + atan((WINDOW_HEIGHT+z)/sqrt(x*x+y*y)))/2.0;
	double lz = z - tan(vfov - atan((WINDOW_HEIGHT - z)/sqrt(x*x+y*y))) * sqrt(x*x+y*y);
	double ratio = (double)width / (double)height;
	double hfov = (atan((WINDOW_HEIGHT*ratio-y)/sqrt(x*x+z*z)) + atan((WINDOW_HEIGHT*ratio+y)/sqrt(x*x+z*z)))/2.0;
	double ly = y - tan(hfov - atan((WINDOW_HEIGHT*ratio - y)/sqrt(x*x+z*z))) * sqrt(x*x+z*z);

	gluLookAt( x, y, z, 0.0, ly, lz, 0.0, 0.0, 1.0);
	double r = sqrt(x*x+y*y+z*z);
	switch(scene) {
	case 0:
	case 1:
		glEnable(GL_FOG);
		glFogf(GL_FOG_START, 5 + r);
		glFogf(GL_FOG_END, 10 + r);
		break;
	case 2:
		glDisable(GL_FOG);
		break;
	}
	draw();
	glutSwapBuffers();
}

void reshape(int w, int h) {
	width = w;
	height = h;
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double x = offset_x + eye_x;
	double y = offset_y + eye_y;
	double z = offset_z + eye_z;
	double vfov = (atan((WINDOW_HEIGHT-z)/sqrt(x*x + y*y)) + atan((WINDOW_HEIGHT+z)/sqrt(x*x + y*y))) / M_PI * 180.0;
	double ratio = (double)width / (double)height;
	double hfov = (atan((WINDOW_HEIGHT*ratio-y)/sqrt(x*x + z*z)) + atan((WINDOW_HEIGHT*ratio+y)/sqrt(x*x + z*z))) / M_PI * 180.0;
	gluPerspective(vfov, hfov/vfov, 0.001, 200.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void redisplay() {
	glutSetWindow(window);
	reshape(width, height);
	glutPostRedisplay();
}

void idle() {
	usleep(10000);
	redisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27:
			exit(0);
			break;
		case '1':
			scene = 0;
			break;
		case '2':
			scene = 1;
			break;
		case '3':
			scene = 2;
			break;
		case 'x':
			offset_x += 0.1;
			break;
		case 'X':
			offset_x -= 0.1;
			break;
		case 'y':
			offset_y += 0.1;
			break;
		case 'Y':
			offset_y -= 0.1;
			break;
		case 'z':
			offset_z += 0.1;
			break;
		case 'Z':
			offset_z -= 0.1;
			break;
	}
}

