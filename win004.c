#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "array.h"

// ルーレット選択
// 配列の要素は非負の整数であること前提
int roulette(int *A, int n) {
    // 合計値を計算
    int s = sum(A, n);
    // 0 以上合計値未満の乱数を生成
    int r = rand() % s;
    int i = 0;
    while (i < n) {
        // 要素を引いていく
        // 大きい値ほど選ばれやすい?
        r -= A[i];
        // 0になったら終了
        if (r <= 0) {
            break;
        }
        i++;
    }
    // 選ばれた添え字を返す
    return i;
}

int main(void) {
    int n = 10;
    int A[n];
    srand((unsigned)time(NULL));
    range(A, n, 0, 1);
    printDecimalArray(A, n);
    printDecimal(roulette(A, n));
    return 0;
}