#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define printDecimal(x) printf("%d\n", x)
#define arrayLength(A) sizeof(A) / sizeof(A[0])

void printDecimalArray(int *A, int n) {
    putchar('{');
    for (int i = 0; i < n; i++) {
        printf("%d", A[i]);
        if (i < n - 1)
            printf(", ");
    }
    printf("}\n");
}

void zeros(int *A, int n) {
    for (int i = 0; i < n; i++)
        A[i] = 0;
}

int sumInt(int *A, int n) {
    int s = 0;
    for (int i = 0; i < n; i++)
        s += A[i];
    return s;
}

// ルーレット選択
// 選択された要素がある添え字を返す
int roulette(int *A, int n) {
    int r = rand() % sumInt(A, n);
    int i;
    // 乱数は非負であるからループに必ず入るはず
    for (i = 0; i < n && r >= 0; i++)
        r -= A[i];
    // iは0でないと思う
    return i - 1;
}

// 配列の要素を1つ削除する
// 削除後の配列は元の配列の長さ-1とする
void delElementInt(int *src, int *dst, int src_len, int index) {
    int j = 0;
    for (int i = 0; i < src_len; i++) {
        // 該当する添え字以外コピー
        if (i != index) {
            dst[j] = src[i];
            j++;
        }
    }
}

// 親を選択する
// parents には長さ2の整数型配列を与える
void chooseParents(int *A, int n, int *parents) {
    // 親1以外の要素を格納する配列
    int B[n - 1];
    // 親1を選択
    parents[0] = roulette(A, n);
    // 親1を配列から除外する
    // ここで次に選ばれる親の添え字がずれることに注意
    delElementInt(A, B, n, parents[0]);
    // 親2を選択
    parents[1] = roulette(B, n - 1);
    // 親1より後ろの要素は添え字が1つずつずれるので修正
    if (parents[1] >= parents[0])
        parents[1]++;
}

int main(void) {
    int n1, t, loop;
    int sample1[] = {1, 2, 3};
    n1 = arrayLength(sample1);
    // ループ数
    loop = 200000;
    int result1[n1];
    int result2[n1];
    int parents[2];
    zeros(result1, n1);
    zeros(result2, n1);
    t = (unsigned)time(NULL);
    printDecimal(t);
    srand(t);
    chooseParents(sample1, n1, parents);
    printDecimalArray(parents, 2);
    for (int i = 0; i < loop; i++) {
        // 親選択
        chooseParents(sample1, n1, parents);
        // 親1と親2に選ばれた回数をそれぞれカウント
        result1[parents[0]]++;
        result2[parents[1]]++;
    }
    printDecimalArray(result1, n1);
    printDecimalArray(result2, n1);
    return 0;
}