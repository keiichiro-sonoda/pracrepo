#include <stdio.h>
#include "array.h"

int main(void) {
    int n = 10;
    int A[n];
    randArray(A, n);
    printDecimalArray(A, n);
    return 0;
}