#include <stdio.h>

// 等差数列で初期化する関数
void range(int *A, int n, int start, int step) {
    for (int i = 0; i < n; i++) {
        A[i] = start + i * step;
    }
}

int main(void) {
    int size = 10;
    int i, a[size], b[size], c[size];

    range(a, size, 1, 1);
    range(b, size, 1, -1);

    for (i = 0; i < size; i++) {
        c[i] = a[i] + b[i];
    }

    printf("c = [");
    for (i = 0; i < size; i++) {
        printf("%d,", c[i]);
    }

    printf("\b]\n");

    return 0;
}