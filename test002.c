#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// get array length
#define arrayLength(A) sizeof(A) / sizeof(A[0])
// macros for debugging
#define printDecimal(x) printf("%d\n", x)
#define printFloat(x) printf("%.6f\n", x)

// functions for debugging
void printIntArray(const int *A, int n) {
    int i;
    putchar('{');
    for (i = 0; i < n; i++) {
        printf("%d", A[i]);
        if (i < n - 1)
            printf(", ");
    }
    printf("}\n");
}

void printFloatArray(const float *A, int n) {
    int i;
    putchar('{');
    for (i = 0; i < n; i++) {
        printf("%.2f", A[i]);
        if (i < n - 1)
            printf(", ");
    }
    printf("}\n");
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

// global variable
int *GLOBAL;

// make an array of random numbers
void arrayRandom(int n);

void expArray(const float *X, float *Y, int n) {
    for (int i = 0; i < n; i++)
        Y[i] = (float)exp(X[i]);
}

// set all elements of an array to 0
void zeros(int *A, int n) {
    for (int i = 0; i < n; i++)
        A[i] = 0;
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
    printFloat(r);
    return 0;
}

int main(void) {
    srand((unsigned)time(NULL));
    int sample1[] = {5, 7, 6, 8, 2, 1, 9, 3, 4, 0, 11, 6, 7, 12, 1, 14, 9, 10};
    float sample2[] = {-0.2, -0.1, 0.0, 0.1, 0.3, 1.0};
    int l1, l2;
    l1 = arrayLength(sample1);
    l2 = arrayLength(sample2);
    float sample3[l2];
    int s;
    int result1[l1];
    float s2;
    // initialize
    zeros(result1, l1);
    printf("length = %d\n", l1);
    arrayRandom(l1);
    printIntArray(GLOBAL, l1);
    s = sumInt(GLOBAL, l1);
    for (int i = 0; i < s * 100; i++) {
        result1[rouletteInt(GLOBAL, l1, s)]++;
    }
    printIntArray(result1, l1);
    printFloatArray(sample2, l2);
    expArray(sample2, sample3, l2);
    printFloatArray(sample3, l2);
    s2 = sumFloat(sample3, l2);
    printFloat(s2);
    // free memory
    free(GLOBAL);
    if (GLOBAL == NULL) {
        printf("GLOBAL is NULL\n");
    }
    return 0;
}

void arrayRandom(int n) {
    GLOBAL = (int *)malloc(n * sizeof(int));
    // failed
    if (GLOBAL == NULL) return;
    for (int i = 0; i < n; i++)
        // 0-100
        GLOBAL[i] = rand() % 100;
}