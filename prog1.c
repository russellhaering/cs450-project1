// Header Files
#include "dataset.h"
#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define HEIGHT 100.0
#define WIDTH 100.0

// Function prototypes
void display();
void initGL();
void HSVtoRGB(float [], float []);

// global variables
DATASET *set;
float grid_width, grid_height;

// Function Bodies
void display() {
  int i, j;
  float x, y, hsv[3], rgb[3];
  long offset;
  hsv[1] = hsv[2] = 1.0;

  glClear(GL_COLOR_BUFFER_BIT);

  for (i = 0; i < set->y_dim; i++) {
    for (j = 0; j < set->x_dim; j++) {
      offset = get_offset(set, i, j);
      // Calculate and set the color for this quad
      if (set->data[offset] == NO_DATA) {
        glColor3f(1.0, 1.0, 1.0);
      }
      else {
        hsv[0] = ((240.0 / 19.0) * (19 - set->data[offset]));
        HSVtoRGB(hsv, rgb);
        glColor3f(rgb[0], rgb[1], rgb[2]);
      }

      // Calculate the location of the quad
      x = (j * grid_width) + 1;
      y = (i * grid_height) + 1;

      // Draw the quad
      glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + grid_width, y);
        glVertex2f(x + grid_width, y + grid_height);
        glVertex2f(x, y + grid_height);
      glEnd();
    }
  }
  glFlush();
}

void initGL() {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, WIDTH, HEIGHT, 0);
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
  float f = (hsv[0] / 60.0f - (int) (hsv[0] / 60.0f));
  float tmp1 = hsv[2] * (1 - hsv[1]);
  float tmp2 = hsv[2] * (1 - hsv[1] * f);
  float tmp3 = hsv[2] * (1 - hsv[1] * (1 - f));
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
      printf("What!? Inconceivable! (%d)\n", (int)(hsv[0] / 60));
  }

}

int main(int argc, char ** argv) {
  if (argc != 2) {
    printf("USAGE: %s <path_to_data_file>\n", argv[0]);
    return 1;
  }

  set = load_dataset(argv[1]);
  if (set == NULL) {
    return 0;
  }

  grid_width = ((WIDTH - 2) / set->x_dim);
  grid_height = ((HEIGHT - 2) / set->y_dim);

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
