#ifndef __DATASET_H
#define __DATASET_H

#define BUCKET_COUNT 20
#define NO_DATA -1

typedef struct {
  long x_dim;
  long y_dim;
  int *data;
} DATASET;

long get_offset(DATASET *set, int row, int col);

DATASET *load_dataset(char *filename);

void free_dataset(DATASET *set);

#endif
