#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "array.h"

// ルーレット選択
int roulette(int *A, int n) {
    return 0;
}

int main(void) {
    int n = 10;
    int A[n];
    srand((unsigned)time(NULL));
    range(A, n, 0, 1);
    printDecimalArray(A, n);
    printDecimal(sum(A, n));
    return 0;
}