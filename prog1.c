/**
 * prog1.c
 *
 * A scientific data visualization tool, created for project 1 in CS 450/550
 * at Oregon State University, Fall 2010.
 *
 * Author: Russell Haering
 */

#include "dataset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define BUCKET_OPTSTR     "--buckets"

#define VIEW_HEIGHT       480
#define VIEW_WIDTH        720
#define MARGIN            7
#define KEY_WIDTH         200
#define KEY_OUTLINE_WIDTH 5

#define WINDOW_HEIGHT     (VIEW_HEIGHT + 2 * MARGIN)
#define WINDOW_WIDTH      (VIEW_WIDTH + 3 * MARGIN + KEY_WIDTH)

// Function prototypes
void contourBottom(int row, int col);
void contourTop(int row, int col);
void contourRight(int row, int col);
void contourLeft(int row, int col);
void display();
void initGL();
void HSVtoRGB(float [], float []);

// global variables
DATASET *set;
float grid_width, grid_height;
int contours, edges;

// Function Bodies
void contourBottom(int row, int col)
{
  float x, y;
  x = (col * grid_width) + MARGIN;
  y = ((row + 1) * grid_height) + MARGIN;
  glVertex2f(x - 1.0, y);
  glVertex2f(x + (grid_width), y);
}

void contourTop(int row, int col)
{
  float x, y;
  x = (col * grid_width) + MARGIN;
  y = (row * grid_height) + MARGIN;
  glVertex2f(x - 1.0, y);
  glVertex2f(x + (grid_width), y);
}

void contourRight(int row, int col)
{
  float x, y;
  x = ((col + 1) * grid_width) + MARGIN;
  y = (row * grid_height) + MARGIN;
  glVertex2f(x, y);
  glVertex2f(x, y + (grid_height));
}

void contourLeft(int row, int col)
{
  float x, y;
  x = (col * grid_width) + MARGIN;
  y = (row * grid_height) + MARGIN;
  glVertex2f(x, y);
  glVertex2f(x, y + (grid_height));
}

void display()
{
  int i, j, val, val2;
  int contour, edge;
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
        hsv[0] = ((240.0 / (set->buckets - 1)) * ((set->buckets - 1) - set->data[offset]));
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
      val = set->data[offset];

      // Draw horizontal contours
      if (i == 0) {
        if (edges && set->data[offset] != NO_DATA) {
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
        val2 = set->data[offset2];

        edge = (val == NO_DATA || val2 == NO_DATA) && (val != val2);
        contour = edge || (val != val2);
        if ((edge && edges) || (contour && contours)) {
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
        val2 = set->data[offset2];

        edge = (val == NO_DATA || val2 == NO_DATA) && (val != val2);
        contour = edge || (val != val2);
        if ((edge && edges) || (contour && contours)) {
          contourRight(i, j);
        }
      }
    }
  }
  glEnd();


  // Draw the key background
  glBegin(GL_QUADS);
  bucket_height = (((float) (VIEW_HEIGHT - (2 * KEY_OUTLINE_WIDTH))) / set->buckets);
  for (i = 0; i < set->buckets; i++) {
    hsv[0] = ((240.0 / (set->buckets - 1)) * i);
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

void initGL()
{
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
void HSVtoRGB(float hsv[3], float rgb[3])
{
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
      printf("HSVtoRGB Error: (%d)\n", (int)(hsv[0]));
  }

}

void print_usage(char *prog) {
  printf("USAGE: %s [%s <bucket_count>] <path_to_data_file>\n", prog, BUCKET_OPTSTR);
}

int main(int argc, char ** argv)
{
  long buckets;
  char *file;

  if (argc != 2 && argc != 4) {
    print_usage(argv[0]);
    return 1;
  }

  if (argc == 4) {
    // Verify
    if (strcmp(BUCKET_OPTSTR, argv[1])) {
      print_usage(argv[0]);
      return 1;
    }

    buckets = strtol(argv[2], NULL, 10);
    if (buckets <= 0) {
      printf("Invalid bucket value '%s'\n", argv[2]);
      return 1;
    }

    file = argv[3];
  }

  else {
    buckets = 20;
    file = argv[1];
  }

  if (buckets < 200) {
    contours = 1;
  }
  else {
    contours = 0;
  }
  edges = 1;

  set = load_dataset(file, buckets);
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
 
  free(set);
  return 0;
}
