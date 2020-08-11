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

// ルーレット選択（負の値も考慮）
// ネイピア数の累乗で正の値にする
// 与えるのはfloat型の配列
int rouletteExp(float *A, int n) {
    float r, EA[n];
    // 指数の配列を作る
    expArray(A, EA, n);
    //printFloatArray(EA, n);
    //printFloat(sumFloat(EA, n));
    r = (float)rand() / 35536 * sumFloat(EA, n);
    int i;
    for (i = 0; i < n && r > 0; i++) {
        r -= A[i];
    }
    return i - 1;
}

int main(void) {
    int n = 10;
    int A[n];
    float B[] = {-4.5, -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5, 4.5};
    int result[n];
    int r;
    srand((unsigned)time(NULL));
    range(A, n, 0, 1);
    printDecimalArray(A, n);
    printFloatArray(B, n);
    printDecimal(roulette(A, n));
    // 初期化を忘れていた
    zeros(result, n);
    for (int i = 0; i < 450000; i++) {
        r = roulette(A, n);
        result[r]++;
    }
    printDecimalArray(result, n);
    // 再度0で初期化
    zeros(result, n);
    for (int i = 0; i < 100000; i++) {
        result[rouletteExp(B, n)]++;
    }
    printDecimalArray(result, n);
    return 0;
}