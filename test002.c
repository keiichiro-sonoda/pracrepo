#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void printIntArray(const int *A, int n) {
    int i;
    for (i = 0; i < n; i++)
        printf("%d ", A[i]);
    putchar('\n');
    return;
}

void printBin(int x) {
    int A[64];
    int i, j;
    for (i = 0; x != 0; i++) {
        A[i] = x & 1;
        x >>= 1;
    }
    for (j = i - 1; j >= 0; j--)
        printf("%d", A[j]);
    putchar('\n');
    return;
}

int *makeArrayRand(int n) {
    int *A;
    int i;
    A = (int *)malloc(n * sizeof(int));
    for (i = 0; i < n; i++) {
        A[i] = rand() % 100;
    }
    return A;
}

void insertionSortRecursive(int *A, int n) {
    if (n > 1) {
        insertionSortRecursive(A, n - 1);
        int key = A[n - 1];
        int i;
        for (i = n - 2; i >= 0 && A[i] > key; i--)
            A[i + 1] = A[i];
        A[i + 1] = key;
    }
    return;
}

int main(void) {
    srand((unsigned)time(NULL));
    int r;
    int l = 16;
    int *sample1 = makeArrayRand(l);
    r = rand();
    printf("%ld\n", sizeof r);
    printf("%d\n", r);
    printf("%x\n", r);
    printBin(r);
    printIntArray(sample1, l);
    insertionSortRecursive(sample1, l);
    printIntArray(sample1, l);
    return 0;
}