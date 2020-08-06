#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "array.h"

int main(void) {
    int n = 10;
    int A[n];
    randArray(A, n);
    srand((unsigned)time(NULL));
    printDecimalArray(A, n);
    return 0;
}