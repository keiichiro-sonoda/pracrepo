#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "array.h"

int main(void) {
    int n = 10;
    int A[n];
    printDecimal(time(NULL));
    srand((unsigned)time(NULL));
    randArray(A, n);
    printDecimalArray(A, n);
    return 0;
}