#ifndef __DATASET_H
#define __DATASET_H

#define BUCKET_COUNT 20

typedef struct {
  long x_dim;
  long y_dim;
  int **grid;
  int *data;
} DATASET;

DATASET *load_dataset(char *filename);

void free_dataset(DATASET *set);

#endif
