#include <stdio.h>
#include "sort01.h"

// for debugging
void sortTest(void) {
    int sample1[] = {5, 6, 8, 1, 2, 10, 3, 4, 2, 10, 9, 7, 20, 0, -2};
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

// print an array of decimal numbers
void printDecimalArray(const int *A, int n) {
    int i;
    putchar('{');
    for (i = 0; i < n; i++) {
        printf("%3d", A[i]);
        if (i < n - 1) {
            printf(", ");
        }
    }
    printf("}\n");
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
