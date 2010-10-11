/**
 * dataset.h
 *
 * A scientific data visualization tool, created for project 1 in CS 450/550
 * at Oregon State University, Fall 2010.
 *
 * Author: Russell Haering
 */

#ifndef __DATASET_H
#define __DATASET_H

#define NO_DATA -1

typedef struct {
  long buckets;
  long x_dim;
  long y_dim;
  int *data;
} DATASET;

long get_offset(DATASET *set, int row, int col);

DATASET *load_dataset(char *filename, long buckets);

void free_dataset(DATASET *set);

#endif
