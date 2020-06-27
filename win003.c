#include <stdio.h>
#include "array.h" // <> ではなく "" で囲う

// array.h と array.c のおかげでシンプルになった!
// コンパイル時に array.c も組み合わせなければならないので
// Makefile を活用するべき?
int main(void) {
    int size; size = 5;

    int A[size]; range(A, size, 1, 1);
    int B[size]; range(B, size, 1, -1);
    int C[size]; zeros(C, size);

    printf("A = ");
    printDecimalArray(A, size);
    printf("B = ");
    printDecimalArray(B, size);
    printf("C = ");
    printDecimalArray(C, size);

    for (int i = 0; i < size; i++) {
        C[i] = A[i] + B[i];
    }

    printf("\nC = ");
    printDecimalArray(C, size);

    return 0;
}
