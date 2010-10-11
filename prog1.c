// Header Files
#include "dataset.h"
#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define VIEW_HEIGHT 480
#define VIEW_WIDTH  720
#define MARGIN 7
#define KEY_WIDTH 200

#define WINDOW_HEIGHT (VIEW_HEIGHT + 2 * MARGIN)
#define WINDOW_WIDTH (VIEW_WIDTH + 3 * MARGIN + KEY_WIDTH)

#define KEY_OUTLINE_WIDTH 5


// Function prototypes
void display();
void initGL();
void HSVtoRGB(float [], float []);

// global variables
DATASET *set;
float grid_width, grid_height;

// Function Bodies
void contourBottom(int row, int col) {
  float x, y;
  x = (col * grid_width) + MARGIN;
  y = ((row + 1) * grid_height) + MARGIN;
  glVertex2f(x - 1.0, y);
  glVertex2f(x + (grid_width), y);
}

void contourTop(int row, int col) {
  float x, y;
  x = (col * grid_width) + MARGIN;
  y = (row * grid_height) + MARGIN;
  glVertex2f(x - 1.0, y);
  glVertex2f(x + (grid_width), y);
}

void contourRight(int row, int col) {
  float x, y;
  x = ((col + 1) * grid_width) + MARGIN;
  y = (row * grid_height) + MARGIN;
  glVertex2f(x, y);
  glVertex2f(x, y + (grid_height));
}

void contourLeft(int row, int col) {
  float x, y;
  x = (col * grid_width) + MARGIN;
  y = (row * grid_height) + MARGIN;
  glVertex2f(x, y);
  glVertex2f(x, y + (grid_height));
}

void display() {
  int i, j;
  float x, y, hsv[3], rgb[3];
  long offset, offset2;
  float bucket_height;
  hsv[1] = hsv[2] = 1.0;

  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the colored squares
  glBegin(GL_QUADS);
  for (i = 0; i < set->y_dim; i++) {
    for (j = 0; j < set->x_dim; j++) {
      offset = get_offset(set, i, j);

      // Calculate and set the color for this quad
      if (set->data[offset] == NO_DATA) {
        glColor3f(1.0, 1.0, 1.0);
      }
      else {
        hsv[0] = ((240.0 / (BUCKET_COUNT - 1)) * ((BUCKET_COUNT - 1) - set->data[offset]));
        HSVtoRGB(hsv, rgb);
        glColor3f(rgb[0], rgb[1], rgb[2]);
      }

      // Calculate the location of the quad
      x = (j * grid_width) + MARGIN;
      y = (i * grid_height) + MARGIN;

      // Draw the quad
      glVertex2f(x, y);
      glVertex2f(x + grid_width, y);
      glVertex2f(x + grid_width, y + grid_height);
      glVertex2f(x, y + grid_height);
    }
  }
  glEnd();

  // Draw the countours
  glBegin(GL_LINES);
  glColor3f(0, 0, 0);
  for (i = 0; i < set->y_dim; i++) {
    for (j = 0; j < set->x_dim; j++) {
      offset = get_offset(set, i, j);

      // Draw horizontal contours
      if (i == 0) {
        if (set->data[offset] != NO_DATA) {
          contourTop(i, j);
        }
      }
      if (i == (set->y_dim - 1)) {
        if (set->data[offset] != NO_DATA) {
          contourBottom(i, j);
        }
      }
      else {
        offset2 = get_offset(set, i + 1, j);
        if (set->data[offset] != set->data[offset2]) {
          contourBottom(i, j);
        }
      }

      // Draw vertical contours
      if (j == 0) {
        if (set->data[offset] != NO_DATA) {
          contourLeft(i, j);
        }
      }
      if (j == (set->x_dim - 1)) {
        if (set->data[offset] != NO_DATA) {
          contourRight(i, j);
        }
      }
      else {
        offset2 = get_offset(set, i, j + 1);
        if (set->data[offset] != set->data[offset2]) {
          contourRight(i, j);
        }
      }
    }
  }
  glEnd();


  // Draw the key background
  glBegin(GL_QUADS);
  bucket_height = (((float) (VIEW_HEIGHT - (2 * KEY_OUTLINE_WIDTH))) / BUCKET_COUNT);
  for (i = 0; i < BUCKET_COUNT; i++) {
    hsv[0] = ((240.0 / (BUCKET_COUNT - 1)) * i);
    HSVtoRGB(hsv, rgb);
    glColor3f(rgb[0], rgb[1], rgb[2]);
    glVertex2f(VIEW_WIDTH + 2 * MARGIN + KEY_OUTLINE_WIDTH, MARGIN + KEY_OUTLINE_WIDTH + i * bucket_height);
    glVertex2f(VIEW_WIDTH + 2 * MARGIN + KEY_OUTLINE_WIDTH, MARGIN + KEY_OUTLINE_WIDTH + (i + 1) * bucket_height);
    glVertex2f(WINDOW_WIDTH - MARGIN - KEY_OUTLINE_WIDTH, MARGIN + KEY_OUTLINE_WIDTH + (i + 1) * bucket_height);
    glVertex2f(WINDOW_WIDTH - MARGIN - KEY_OUTLINE_WIDTH, MARGIN + KEY_OUTLINE_WIDTH + i * bucket_height);
  }
  glEnd();

  // Draw the key outline
  glColor3f(0.0, 0.0, 0.0);
  glBegin(GL_QUADS);
    // Left Side
    glVertex2f(VIEW_WIDTH + 2 * MARGIN, MARGIN);
    glVertex2f(VIEW_WIDTH + 2 * MARGIN, VIEW_HEIGHT + MARGIN);
    glVertex2f(VIEW_WIDTH + 2 * MARGIN + KEY_OUTLINE_WIDTH, VIEW_HEIGHT + MARGIN);
    glVertex2f(VIEW_WIDTH + 2 * MARGIN + KEY_OUTLINE_WIDTH, MARGIN);

    // Right Side
    glVertex2f(WINDOW_WIDTH - MARGIN - KEY_OUTLINE_WIDTH, MARGIN);
    glVertex2f(WINDOW_WIDTH - MARGIN - KEY_OUTLINE_WIDTH, VIEW_HEIGHT + MARGIN);
    glVertex2f(WINDOW_WIDTH - MARGIN, VIEW_HEIGHT + MARGIN);
    glVertex2f(WINDOW_WIDTH - MARGIN, MARGIN);

    // Top
    glVertex2f(VIEW_WIDTH + 2 * MARGIN, MARGIN);
    glVertex2f(VIEW_WIDTH + 2 * MARGIN, MARGIN + KEY_OUTLINE_WIDTH);
    glVertex2f(WINDOW_WIDTH - MARGIN, MARGIN + KEY_OUTLINE_WIDTH);
    glVertex2f(WINDOW_WIDTH - MARGIN, MARGIN);

    // Bottom
    glVertex2f(VIEW_WIDTH + 2 * MARGIN, WINDOW_HEIGHT - MARGIN - KEY_OUTLINE_WIDTH);
    glVertex2f(VIEW_WIDTH + 2 * MARGIN, WINDOW_HEIGHT - MARGIN);
    glVertex2f(WINDOW_WIDTH - MARGIN, WINDOW_HEIGHT - MARGIN);
    glVertex2f(WINDOW_WIDTH - MARGIN, WINDOW_HEIGHT - MARGIN - KEY_OUTLINE_WIDTH);
  glEnd();
  glFlush();
}

void initGL() {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  glMatrixMode(GL_MODELVIEW);
  glDepthFunc(GL_NEVER);
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

  grid_width = ((float) VIEW_WIDTH / set->x_dim);
  grid_height = ((float) VIEW_HEIGHT / set->y_dim);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("CS 450/550 Project 1");
  glutDisplayFunc(display);

  initGL();
  glutMainLoop();
  return 0;
}
