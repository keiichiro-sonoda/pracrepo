#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "sort01.h"
#include "othello.h"

// insertion sort
void insertionSort(int *A, int n) {
    int i, j, key;
    for (j = 1; j < n; j++) {
        key = A[j];
        // A[j]をソート済みの列A[1..j-1]に挿入する.
        for (i = j - 1; i >= 0 && A[i] > key; i--)
            A[i + 1] = A[i];
        A[i + 1] = key;
    }
}

// global variable
int *GLOBAL;

// make an array of random numbers
void arrayRandom(int n) {
    GLOBAL = (int *)malloc(n * sizeof(int));
    // failed
    if (GLOBAL == NULL) return;
    for (int i = 0; i < n; i++)
        // 0-100
        GLOBAL[i] = rand() % 100;
}

void expArray(const float *X, float *Y, int n) {
    for (int i = 0; i < n; i++)
        Y[i] = (float)exp(X[i]);
}

// return the sum of an integer array
int sumInt(const int *A, int n) {
    int s = 0;
    for (int i = 0; i < n; i++)
        s += A[i];
    return s;
}

// return the sum of a floating point array
float sumFloat(const float *A, int n) {
    float s = 0.0;
    for (int i = 0; i < n; i++)
        s += A[i];
    return s;
}

// roulette selection
// only supports non-negative integers
// return an index
int rouletteInt(const int *A, int n, int s) {
    int r = rand() % s;
    int i;
    for (i = 0; i < n - 1; i++) {
        r -= A[i];
        if (r < 0)
            return i;
    }
    return i;
}

// roulette selection
// only supports non-negative floating point numbers
int rouletteFloat(const float *A, int n, float s) {
    float r = (float)rand() / RAND_MAX * s;
    int i;
    for (i = 0; i < n - 1; i++) {
        r -= A[i];
        if (r < 0)
            return i;
    }
    return i;
}

// for debugging
void sortTest(void) {
    int sample1[] = {5, 6, 8, 1, 2, 10, 3, 4, 2, 10, 9, 7, 20, 0, -2, -1};
    int n = arrayLength(sample1);
    int indexes[n];
    int i;
    for (i = 0; i < n; i++) {
        indexes[i] = i;
    }
    printf("before\n");
    printDecimalArray(sample1, n);
    quicksortDD(sample1, indexes, 0, n - 1);
    printf("after\n");
    printDecimalArray(sample1, n);
    printDecimalArray(indexes, n);
}

// exchange A[i] and A[j]
int exchange(int *A, int i, int j) {
    int t = A[i];
    A[i] = A[j];
    A[j] = t;
    return 0;
}

int partitionDD(int *A, int *B, int p, int r) {
    int i, j;
    int x = A[r];
    i = p;
    for (j = p; j < r; j++) {
        if (A[j] > x) {
            exchange(A, i, j);
            exchange(B, i, j);
            i++;
        }
    }
    exchange(A, i, r);
    exchange(B, i, r);
    return i;
}

// descending order
void quicksortDD(int *A, int *B, int p, int r) {
    if (p < r) {
        int q = partitionDD(A, B, p, r);
        quicksortDD(A, B, p, q - 1);
        quicksortDD(A, B, q + 1, r);
    }
}