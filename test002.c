// this is a source file for debugging
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sort01.h"

/* pf の引数は，「戻り値void，int1個の引数をとる関数」へのポインタと，「整数」 */
void call_func(void (*pf)(int), int x) {
    pf(x);    /* 関数ポインタ pf を使った関数呼び出し */
}

void func_A(int n) {
    printf("by func_A, n^2 = %d\n", n*n);
}

void func_B(int n) {
    printf("by func_B, n^3 = %d\n", n*n*n);
}

int main(void) {
    /* 例1：関数を指すポインタ変数 */
    void (*p)(int);     /* ポインタ変数を宣言 */

    p = func_A;
    printf("p = 0x%p, ", p);  /* pointer の指すアドレスを表示 */
    p(2);                   /* ポインタを使って関数呼び出し */

    p = func_B;
    printf("p = 0x%p, ", p);
    p(2);

    /* 例2：関数のアドレスを別の関数の引数として渡す． */
    /* 同じデータに対して，違う処理をしたい場合に使える． */
    int n = 10;
    call_func(func_A, n);   /* 関数名 func_A を引数として呼び出し． */
    call_func(func_B, n);   /* 関数名 func_B を引数として呼び出し． */

    sortTest();
    return 0;
}