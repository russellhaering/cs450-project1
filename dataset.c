/**
 * dataset.c
 *
 * A scientific data visualization tool, created for project 1 in CS 450/550
 * at Oregon State University, Fall 2010.
 *
 * Author: Russell Haering
 */

#include "dataset.h"
#include <ctype.h>
#include <stdio.h>
#include <float.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

long get_offset(DATASET *set, int row, int col)
{
  return ((set->x_dim * row) + col);
}

DATASET *load_dataset(char *filename)
{
  int fd;
  long i, j;
  off_t fsize;
  struct stat stats;
  DATASET *set;
  float *data, largest, smallest;
  char *fbuf, *bptr;

  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    perror("Error opening data file");
    return NULL;
  }

  if (fstat(fd, &stats) < 0) {
    perror("Error stat-ing data file");
    return NULL;
  }

  fsize = stats.st_size;
  fbuf = mmap(0, fsize, PROT_READ, MAP_SHARED, fd, 0);

  if (fbuf == MAP_FAILED) {
    perror("Error mmap-ing file");
    return NULL;
  }

  madvise(fbuf, fsize, MADV_SEQUENTIAL);
  set = malloc(sizeof(DATASET));

  if (set == NULL) {
    perror("Error allocating space for dataset metadata");
    munmap(fbuf, fsize);
    return NULL;
  }

  bptr = fbuf;

  // Consume # and whitespace
  while (*bptr == '#' || isspace(*bptr)) {
    bptr++;
  }

  // Read the dimensions
  set->x_dim = strtol(bptr, &bptr, 10);
  if (set->x_dim == 0) {
    perror("Error reading X dimension");
    munmap(fbuf, fsize);
    free(set);
    return NULL;
  }

  set->y_dim = strtol(bptr, &bptr, 10);
  if (set->y_dim == 0) {
    perror("Error reading Y dimension");
    munmap(fbuf, fsize);
    free(set);
    return NULL;
  }

  // Allocate a bunch of space
  data = malloc(set->y_dim * set->x_dim * sizeof(float));
  if (data == NULL) {
    perror("Error allocating space for dataset");
    munmap(fbuf, fsize);
    free(set);
    return NULL;
  }

  set->data = malloc(set->y_dim * set->x_dim * sizeof(int));
  if (set->data == NULL) {
    perror("Error allocating space for binned dataset");
    munmap(fbuf, fsize);
    free(set);
    free(data);
    return NULL;
  }

  largest = FLT_MIN;
  smallest = FLT_MAX;

  for (i = 0; i < (set->y_dim * set->x_dim); i++) {
    char *pptr = bptr;
    data[i] = strtof(bptr, &bptr);
    if (bptr == pptr) {
      perror("Error reading data value");
      free_dataset(set);
      munmap(fbuf, fsize);
      return NULL;
    }

    if (data[i] != 0 && data[i] < smallest) {
      smallest = data[i];
    }
    if (data[i] != 0 && data[i] > largest) {
      largest = data[i];
    }
  }

  munmap(fbuf, fsize);

  for (i = 0; i < set->y_dim; i++) {
    for (j = 0; j < set->x_dim; j++) {
      long offset = get_offset(set, (set->y_dim - i - 1), j);
      float val = data[(i * set->x_dim) + j];
      if (val == 0) {
        set->data[offset] = NO_DATA;
      }
      else {
        set->data[offset] =
          (int) (((val - smallest) / (largest - smallest)) * (BUCKET_COUNT - 1) + 0.5);

      }
    }
  }

  free(data);
  return set;
}

void free_dataset(DATASET *set)
{
  free(set->data);
  free(set);
}
