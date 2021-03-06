#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "array.h" // ここで必ず"array.h"をincludeする

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

// float型の配列を表示
void printFloatArray(const float *A, int n) {
    putchar('{');
    for (int i = 0; i < n; i++) {
        printf("%5.2f", A[i]);
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

// 等差数列（float）
// 直前の要素に公差を足す方法を用いてみた
void rangeFloat(float *A, int n, float start, float step) {
    A[0] = start;
    for (int i = 1; i < n; i++) {
        A[i] = A[i - 1] + step;
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

// 配列に乱数を代入
void randArray(int *A, int n) {
    for (int i = 0; i < n; i++) {
        A[i] = rand();
    }
}

// Y = exp(X) となる配列 Y を作る
void expArray(float *X, float *Y, int n) {
    for (int i = 0; i < n; i++) {
        Y[i] = expf(X[i]);
    }
}

// 配列の要素の合計を返す
int sum(int *A, int n) {
    int s = 0;
    for (int i = 0; i < n; i++) {
        s += A[i];
    }
    return s;
}

// 配列の要素の合計（float版）
float sumFloat(float *A, int n) {
    float s = 0.0;
    for (int i = 0; i < n; i++) {
        s += A[i];
    }
    return s;
}