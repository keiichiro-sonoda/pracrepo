#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define arrayLength(A) sizeof(A) / sizeof(A[0])

void printIntArray(const int *A, int n) {
    int i;
    for (i = 0; i < n; i++)
        printf("%d ", A[i]);
    putchar('\n');
    return;
}

void insertionSort(int *A, int n) {
    int i, j, key;
    for (j = 1; j < n; j++) {
        key = A[j];
        // A[j]をソート済みの列A[1..j-1]に挿入する.
        for (i = j - 1; i >= 0 && A[i] > key; i--)
            A[i + 1] = A[i];
        A[i + 1] = key;
    }
    return;
}

int *arrayRandom(int n);

int main(void) {
    printf("Hello World!!\n");
    srand((unsigned)time(NULL));
    int sample1[] = {5, 7, 6, 8, 2, 1, 9, 3, 4, 0, 11, 6, 7, 12, 1, 14, 9, 10};
    int l = arrayLength(sample1);
    int *sample2 = arrayRandom(l);
    printIntArray(sample1, l);
    insertionSort(sample1, l);
    printIntArray(sample1, l);
    printIntArray(sample2, l);
    insertionSort(sample2, l);
    printIntArray(sample2, l);
    return 0;
}

int *arrayRandom(int n) {
    int *A;
    A = (int *)malloc(n * sizeof(int));
    if (A == NULL) return NULL;
    int i;
    for (i = 0; i < n; i++)
        A[i] = rand() % 100;
    return A;
}