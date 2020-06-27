#include <stdio.h>

// int型の配列を表示
void printDecimalArray(const int *A, int n) {
    putchar('{');
    for (int i = 0; i < n; i++) {
        printf("%3d", A[i]);
        if (i + 1 < n) {
            printf(", ");
        }
    }
    printf("}\n");
}

// 等差数列で初期化する関数
void range(int *A, int n, int start, int step) {
    for (int i = 0; i < n; i++) {
        A[i] = start + i * step;
    }
}

// 全ての要素を特定の数字で初期化
void init(int *A, int n, int num) {
    for (int i = 0; i < n; i++) {
        A[i] = num;
    }
}

int main(void) {
    int size = 10;
    int i, a[size], b[size], c[size];

    init(c, size, 99);
    range(a, size, 1, 1);
    range(b, size, 1, -1);
    printDecimalArray(a, size);
    printDecimalArray(b, size);
    printDecimalArray(c, size);

    for (i = 0; i < size; i++) {
        c[i] = a[i] + b[i];
    }

    printDecimalArray(c, size);

    return 0;
}