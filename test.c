#include <stdio.h>

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

int main(void) {
    int sample1[] = {5, 7, 6, 8, 2, 1, 9, 3, 4, 0};
    int l = arrayLength(sample1);
    printIntArray(sample1, l);
    insertionSort(sample1, l);
    printIntArray(sample1, l);
    return 0;
}