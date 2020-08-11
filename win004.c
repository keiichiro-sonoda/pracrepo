#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "array.h"

// ルーレット選択
// 配列の要素は非負の整数であること前提
int roulette(int *A, int n) {
    // 1以上合計値以下の乱数を生成
    int r = rand() % sum(A, n) + 1;
    int i;
    // 0以下になったら終了
    for (i = 0; i < n && r > 0; i++) {
        // 要素を引いていく
        // 大きい値ほど選ばれやすい?
        r -= A[i];
    }
    // 0 以下にした値を返す
    return i - 1;
}

// 非負のfloat型前提
// 毎回指数の配列を作るのは非効率
// ただし和は毎回計算する
int rouletteFloat(float *A, int n) {
    // 0以上合計値未満の乱数を生成
    float r = (float)rand() / RAND_MAX * sumFloat(A, n);
    int i;
    // r < 0 でループ終了とすることで, 返り値が -1 になることを防ぐ
    for (i = 0; i < n && r >= 0; i++) {
        r -= A[i];
    }
    return i - 1;
}

// ルーレット選択（負の値も考慮）
// ネイピア数の累乗で正の値にする
// 与えるのはfloat型の配列
int rouletteFloatExp(float *A, int n) {
    float EA[n];
    // 指数の配列を作る
    expArray(A, EA, n);
    return rouletteFloat(EA, n);
}

int main(void) {
    int n = 10;
    int loop = 1000000;
    int A[n];
    float s;
    float B[n], C[n];
    int result[n];
    // 期待値を計算したい
    int expect[n];
    // 時刻でシード指定
    srand((unsigned)time(NULL));
    range(A, n, 0, 1);
    rangeFloat(B, n, -4.0, 1.0);
    zeros(result, n);
    expArray(B, C, n);
    s = sumFloat(C, n);
    printFloatArray(B, n);
    printFloatArray(C, n);
    for (int i = 0; i < n; i++) {
        expect[i] = C[i] / s * loop;
    }
    printDecimalArray(expect, n);
    for (int i = 0; i < loop; i++) {
        result[rouletteFloat(C, n)]++;
    }
    printDecimalArray(result, n);
    return 0;
}