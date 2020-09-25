#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define arrayLength(A) sizeof(A) / sizeof(A[0])

// for debug
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

// global variable
int *GLOBAL;

void arrayRandom(int n);

int main(void) {
    printf("Hello World!!\n");
    srand((unsigned)time(NULL));
    int sample1[] = {5, 7, 6, 8, 2, 1, 9, 3, 4, 0, 11, 6, 7, 12, 1, 14, 9, 10};
    int l = arrayLength(sample1);
    arrayRandom(l);
    // check arrays
    printIntArray(sample1, l);
    insertionSort(sample1, l);
    printIntArray(sample1, l);
    printIntArray(GLOBAL, l);
    insertionSort(GLOBAL, l);
    printIntArray(GLOBAL, l);
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