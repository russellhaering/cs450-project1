// Header Files
#include "dataset.h"
#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Function prototypes
void display();
void initGL();
void HSVtoRGB(float [], float []);

// global variables

// Function Bodies
void display() {
	// Put all your drawing code in here
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_POLYGON);
		glVertex2f(0.25, 0.25);
		glVertex2f(0.25, -0.25);
		glVertex2f(-0.25, -0.25);
		glVertex2f(-0.25, 0.25);
	glEnd();
	glFlush();
}

void initGL() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-0.5, 0.5, -0.5, 0.5);
	glMatrixMode(GL_MODELVIEW);
}

// the HSV color model will be as follows
// h : [0 - 360]
// s : [0 - 1]
// v : [0 - 1]
// If you want it differently (in a 2 * pi scale, 256 instead of 1, etc,
// you'll have to change it yourself.
// rgb is returned in 0-1 scale (ready for color3f)
void HSVtoRGB(float hsv[3], float rgb[3]) {
	float tmp1 = hsv[2] * (1-hsv[1]);
	float tmp2 = hsv[2] * (1-hsv[1] * (hsv[0] / 60.0f - (int) (hsv[0]/60.0f) ));
	float tmp3 = hsv[2] * (1-hsv[1] * (1 - (hsv[0] / 60.0f - (int) (hsv[0]/60.0f) )));
	switch((int)(hsv[0] / 60)) {
		case 0:
			rgb[0] = hsv[2];
			rgb[1] = tmp3;
			rgb[2] = tmp1;
			break;
		case 1:
			rgb[0] = tmp2;
			rgb[1] = hsv[2];
			rgb[2] = tmp1;
			break;
		case 2:
			rgb[0] = tmp1;
			rgb[1] = hsv[2];
			rgb[2] = tmp3;
			break;
		case 3:
			rgb[0] = tmp1;
			rgb[1] = tmp2;
			rgb[2] = hsv[2];
			break;
		case 4:
			rgb[0] = tmp3;
			rgb[1] = tmp1;
			rgb[2] = hsv[2];
			break;
		case 5:
			rgb[0] = hsv[2];
			rgb[1] = tmp1;
			rgb[2] = tmp2;
			break;
		default:
			printf("What!? Inconceivable!\n");
	}

}



int main(int argc, char ** argv) {
  if (argc != 2) {
    printf("USAGE: %s <path_to_data_file>\n", argv[0]);
    return 1;
  }

  load_dataset(argv[1]);

	// Example showing how HSVtoRGB works
	float rgb[] = {0, 0, 0};
	float hsv[] = {222, 81/100.0, 84/100.0};
	HSVtoRGB(hsv,rgb);
	printf("HSV: %f %f %f to RGB: %f %f %f\n", hsv[0], hsv[1], hsv[2], rgb[0], rgb[1], rgb[2]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("CS 450/550 Project 1");
	glutDisplayFunc(display);

	initGL();
	glutMainLoop();
  return 0;
}
