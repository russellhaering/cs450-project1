#include "dataset.h"
#include <stdio.h>
#include <float.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

DATASET *load_dataset(char *filename)
{
  int fd;
  long i;
  off_t fsize;
  struct stat stats;
  DATASET *set;
  float *data, largest, smallest;
  char *fbuf, *bptr, *eptr;
  
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
    free(data);
    free(set);
    return NULL;
  }

  set->grid = malloc(set->y_dim * sizeof(int*));
  if (set->grid == NULL) {
    perror("Error allocating space for dataset mapping");
    munmap(fbuf, fsize);
    free(set->data);
    free(set);
    return NULL;
  }

  for (i = 0; i < set->y_dim; i++) {
    set->grid[i] = set->data + (i * set->x_dim * sizeof(float));
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

    if (data[i] < smallest) {
      smallest = data[i];
    }
    if (data[i] > largest) {
      largest = data[i];
    }
  }
  
  munmap(fbuf, fsize);

  for (i = 0; i < (set->y_dim * set->x_dim); i++) {
    set->data[i] = (int) (((data[i] - smallest) / (largest - smallest)) * (BUCKET_COUNT - 1) + 0.5);
  }

  free(data);
  return set;
}

void free_dataset(DATASET *set)
{
  int i;
  free(set->data);
  free(set->grid);
  free(set);
}
