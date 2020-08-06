#include <stdio.h>
#include "array.h"

int main(void) {
    int n = 10;
    int A[n];
    range(A, n, 0, 1);
    printDecimalArray(A, n);
    return 0;
}