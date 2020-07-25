#ifndef SORT01_H
#define SORT01_H

#define arrayLength(A) sizeof(A) / sizeof(A[0])

// exchange A[i] and A[j]
int exchange(int *A, int i, int j);

int partitionDD(int *A, int *B, int p, int r);
// descending order
void quicksortDD(int *A, int *B, int p, int r);
// debugging function
void sortTest(void);

#endif