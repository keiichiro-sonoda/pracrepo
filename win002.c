#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define kugiri() printf("---------------------------------\n")
#define printSize(x) printf("%ld\n", sizeof x)

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

int *makeArrayRand3Digits(int n) {
    int *A;
    int i;
    A = (int *)malloc(n * sizeof(int));
    if (A == NULL) {
        printf("can't malloc.\n");
        return NULL;
    }
    for (i = 0; i < n; i++) {
        A[i] = rand() % 1000;
    }
    return A;
}

int main(void) {
    srand((unsigned)time(NULL));
    int l = 100;
    int *sample1 = makeArrayRand3Digits(l);
    printIntArray(sample1, l);
    return 0;
}