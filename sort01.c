#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "sort01.h"
#include "othello.h"

// global variable
int *GLOBAL;

// for debugging
void sortTest(void) {
    srand(123U);
    srand((unsigned)time(NULL));
    int sample1[] = {5, 6, 8, 1, 2, 10, 3, 4, 2, 10, 9, 7, 20, 0, -2, -1};
    float sample2[] = {-1.0, -0.5, -0.2, 0.0, 0.1, 0.3, 1.0, 2.5, 4.0};
    int l2 = arrayLength(sample2);
    float sample3[l2];
    int l3 = 5;
    int result1[l3];
    expArray(sample2, sample3, l2);
    printFloatArray(sample3, l2);
    rouletteFloatMltDep(sample3, l2, result1, l3);
    printDecimalArray(result1, l3);
    indices(result1, l3);
    printDecimalArray(result1, l3);
    fixIndices(result1, l3);
    printDecimalArray(result1, l3);
}

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

// make an array of random numbers
void arrayRandom(int n) {
    GLOBAL = (int *)malloc(n * sizeof(int));
    // failed
    if (GLOBAL == NULL) return;
    for (int i = 0; i < n; i++)
        // 0-100
        GLOBAL[i] = rand() % 100;
}

void globalTest(void) {
    int n = 10;
    arrayRandom(n);
    printDecimalArray(GLOBAL, n);
    insertionSort(GLOBAL, n);
    printDecimalArray(GLOBAL, n);
    free(GLOBAL);
}

// make an array of exponents
// to make the numbers positive
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

// delete the element of the specified index
// the size of the array doesn't change
void delFloat(float *A, int n, int index) {
    for (int i = 0; i < n - 1; i++)
        if (i >= index)
            A[i] = A[i + 1];
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

// fix the indices misalignment
void fixIndices(int *A, int n) {
    for (int i = n - 2; i >= 0; i--)
        for (int j = i + 1; j < n; j++)
            if (A[i] <= A[j])
                A[j]++;
}

// choose some elements with roulette
// don't allow duplication
void rouletteFloatMltDep(const float *A, int A_len, int *rslt, int rslt_len) {
    // initialize
    zeros(rslt, rslt_len);
    float now[A_len], next[A_len];
    copyArray(A, now, A_len);
    int now_len;
    float s = sumFloat(A, A_len);
    printFloat(s);
    for (int i = 0; i < rslt_len; i++) {
        now_len = A_len - i;
        rslt[i] = rouletteFloat(now, now_len, s);
        s -= now[rslt[i]];
        // delete the selected element
        delFloat(now, now_len, rslt[i]);
        printFloatArray(now, now_len - 1);
        printFloat(s);
    }
    printDecimalArray(rslt, rslt_len);
    // fix the results
    fixIndices(rslt, rslt_len);
}

// check if rouletteFloat workes as expected
void rouletteFloatTest(const float *A, int n) {
    float s = sumFloat(A, n);
    int loop = s * 10000;
    printDecimal(loop);
    int results[n];
    // initialize
    zeros(results, n);
    for (int i = 0; i < loop; i++)
        results[rouletteFloat(A, n, s)]++;
    printDecimalArray(results, n);
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
// also rearrange the indices
void quicksortDD(int *A, int *B, int p, int r) {
    if (p < r) {
        int q = partitionDD(A, B, p, r);
        quicksortDD(A, B, p, q - 1);
        quicksortDD(A, B, q + 1, r);
    }
}