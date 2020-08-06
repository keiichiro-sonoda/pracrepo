#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "array.h"

int main(void) {
    int n = 10;
    int A[n];
    srand((unsigned)time(NULL));
    range(A, n, 0, 1);
    printDecimalArray(A, n);
    return 0;
}