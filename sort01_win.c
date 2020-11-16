#include <stdio.h>
#include <stdlib.h>
#include "othello_win.h"

void sortTest(void) {
    printString("debugging");
    int sample1[] = {5, 1, 3, 6, 8, 2, 9, 0, 4, 7};
    int l1 = arrayLength(sample1);
    printDecimalArray(sample1, l1);
}

int exchange(int *A, int i, int j) {
    int t = A[i];
    A[i] = A[j];
    A[j] = t;
    return 0;
}

int partitionDD(int *A, int *B, int p, int r) {
    int i, j;
    int x = A[r];
    i = p;
    for (j = p; j < r; j++) {
        if (A[j] > x) {
            exchange(A, i, j);
            exchange(B, i, j);
            i++;
        }
    }
    exchange(A, i, r);
    exchange(B, i, r);
    return i;
}

// descending order
void quicksortDD(int *A, int *B, int p, int r) {
    if (p < r) {
        int q = partitionDD(A, B, p, r);
        quicksortDD(A, B, p, q - 1);
        quicksortDD(A, B, q + 1, r);
    }
}

// 浮動小数点型の配列の和を求める
// return the sum of a floating point array
float sumFloat(const float *A, int n) {
    float s = 0.0f;
    for (int i = 0; i < n; i++)
        s += A[i];
    return s;
}

// ルーレット選択
// 非負の浮動小数点数のみに対応
// s には配列の要素の総和を与える
// roulette selection
// only supports non-negative floating point numbers
int rouletteFloat(const float *A, int n, float s) {
    float r = (float)rand() / RAND_MAX * s;
    int i;
    for (i = 0; i < n - 1; i++) {
        r -= A[i];
        if (r < 0)
            return i;
    }
    return i;
}