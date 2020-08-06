#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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

int main(void) {
    int n = 10;
    int A[n];
    int result[n];
    int r;
    srand((unsigned)time(NULL));
    range(A, n, 0, 1);
    printDecimalArray(A, n);
    printDecimal(roulette(A, n));
    // 初期化を忘れていた
    zeros(result, n);
    for (int i = 0; i < 450000; i++) {
        r = roulette(A, n);
        result[r]++;
    }
    printDecimalArray(result, n);
    return 0;
}