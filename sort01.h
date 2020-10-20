#ifndef SORT01_H
#define SORT01_H

// exchange A[i] and A[j]
int exchange(int *A, int i, int j);

// insertion sort
void insertionSort(int *A, int n);

// make an array of random numbers
void arrayRandom(int n);

void globalTest(void);

void expArray(const float *X, float *Y, int n);

// return the sum of an integer array
int sumInt(const int *A, int n);

// return the sum of a floating point array
float sumFloat(const float *A, int n);

// delete the element of the specified index
// the size of the array doesn't change
void delInt(int *A, int n, int index);

// delete the element of the specified index
// the size of the array doesn't change
void delFloat(float *A, int n, int index);

// roulette selection
// only supports non-negative integers
// return an index
int rouletteInt(const int *A, int n, int s);

// roulette selection
// only supports non-negative floating point numbers
int rouletteFloat(const float *A, int n, float s);

// fix the indices misalignment
void fixIndices(int *A, int n);

// choose some elements with roulette
// don't allow duplication
// only supports non-negative integers
void rouletteIntMltDep(const int *A, int A_len, int *rslt, int rslt_len);

// choose some elements with roulette
// don't allow duplication
void rouletteFloatMltDep(const float *A, int A_len, int *rslt, int rslt_len);

// check if rouletteFloat workes as expected
void rouletteFloatTest(const float *A, int n);

int partitionDD(int *A, int *B, int p, int r);
// descending order
void quicksortDD(int *A, int *B, int p, int r);
// debugging function
void sortTest(void);

#endif