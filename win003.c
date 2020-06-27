#include <stdio.h>

// プロトタイプ宣言
void printDecimalArray(const int *A, int n);
void range(int *A, int n, int start, int step);
void init(int *A, int n, int num);
void zeros(int *A, int n);

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

// int型の配列を表示
// これはデバッグでよく使われるらしい
// 誰もが通る道なのか..
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
// 初項n, 交差0の等差数列で考える(気付かなかった)
void init(int *A, int n, int num) {
    range(A, n, num, 0);
}

// 全零配列特化(せっかくなのでinitを使う)
void zeros(int *A, int n) {
    init(A, n, 0);
}
