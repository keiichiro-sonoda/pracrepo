#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// get array length
#define arrayLength(A) sizeof(A) / sizeof(A[0])
#define printDecimal(x) printf("%d\n", x)

// for debugging
void printIntArray(const int *A, int n) {
    int i;
    putchar('{');
    for (i = 0; i < n; i++) {
        printf("%d", A[i]);
        if (i < n - 1)
            printf(", ");
    }
    printf("}\n");
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

// make an array of random numbers
void arrayRandom(int n);

// return the sum of an integer array
int sumInt(int *A, int n) {
    int s = 0;
    for (int i = 0; i < n; i++)
        s += A[i];
    return s;
}

// roulette selection
// only supports non-negative integers
// return an index
int rouletteInt(int *A, int n, int s) {
    int r = rand() % s;
    printDecimal(s);
    printDecimal(r);
    int i;
    for (i = 0; i < n - 1; i++) {
        r -= A[i];
        if (r < 0)
            return i;
    }
    return i;
}

int main(void) {
    printf("Hello World!!\n");
    srand((unsigned)time(NULL));
    int sample1[] = {5, 7, 6, 8, 2, 1, 9, 3, 4, 0, 11, 6, 7, 12, 1, 14, 9, 10};
    int l = arrayLength(sample1);
    printf("length = %d\n", l);
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
    int s = sumInt(sample1, l);
    printDecimal(rouletteInt(sample1, l, s));
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