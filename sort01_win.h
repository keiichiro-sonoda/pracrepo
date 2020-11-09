#ifndef SORT01_WIN_H
#define SORT01_WIN_h

void sortTest(void);

int exchange(int *A, int i, int j);

int partitionDD(int *A, int *B, int p, int r);

// descending order
void quicksortDD(int *A, int *B, int p, int r);

#endif