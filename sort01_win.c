#include <stdio.h>

#define arrayLength(A) sizeof(A) / sizeof(A[0])

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