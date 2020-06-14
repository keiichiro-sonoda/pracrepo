#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define kugiri() printf("---------------------------------\n")
#define printSize(x) printf("%ld\n", sizeof x)
#define printDecimal(x) printf("%d\n", x)

void printIntArrayRange(const int *A, int p, int r) {
    int i;
    printf("{");
    for (i = p; i <= r; i++) {
        printf("%d", A[i]);
        if (i < r)
            printf(", ");
    }
    printf("}\n");
    return;
}

void printIntArray(const int *A, int n) {
    printIntArrayRange(A, 0, n - 1);
    return;
}

int *makeArrayRand3Digits(int n) {
    int *A;
    int i;
    A = (int *)malloc(n * sizeof(int));
    if (A == NULL) {
        printf("can't malloc.\n");
        return NULL;
    }
    for (i = 0; i < n; i++) {
        A[i] = rand() % 1000;
    }
    return A;
}

void swap(int *A, int i, int j) {
    int tv = A[i];
    A[i] = A[j];
    A[j] = tv;
    return;
}

// a以上b以下の任意の整数を返す.
int randomRange(int a, int b) {
    return rand() % (b - a + 1) + a;
}

int partition(int *A, int p, int r) {
    int j;
    int x = A[r];
    int i = p;
    for (j = p; j < r; j++) {
        if (A[j] < x) {
            swap(A, i, j);
            i++;
        }
    }
    swap(A, i, r);
    return i;
}

int randomizedPartition(int *A, int p, int r) {
    int i = randomRange(p, r);
    //printDecimal(A[i]);
    swap(A, i, r);
    return partition(A, p, r);
}

void randomizedQuicksort(int *A, int p, int r) {
    if (p < r) {
        int q = randomizedPartition(A, p, r);
        randomizedQuicksort(A, p, q - 1);
        randomizedQuicksort(A, q + 1, r);
    }
    return;
}

// 重複排除関数
// A: ソート済み配列, n: 配列長
int deduplication(int *A, int n) {
    int i;
    int dup_count = 0;
    for (i = 1; i < n; i++) {
        // 重複発見
        if (A[i - 1] == A[i]) {
            dup_count++;
        }
        // 今までの重複数だけ前にずらす
        A[i - dup_count] = A[i];
    }
    // 重複を含まない部分の長さを返す
    return n - dup_count;
}

int binarySearchRecursive(const int *A, int p, int r, int v) {
    if (p <= r) {
        int q = (p + r) / 2;
        if (A[q] < v)
            return binarySearchRecursive(A, q + 1, r, v);
        if (A[q] > v)
            return binarySearchRecursive(A, p, q - 1, v);
        else
            return q;
    }
    return -1;
}

int binarySearchRecursive2(const int *A, int p, int r, int v) {
    if (p <= r) {
        int q = (p + r) / 2;
        if (A[q] < v)
            return binarySearchRecursive2(A, q + 1, r, v);
        if (A[q] > v)
            return binarySearchRecursive2(A, p, q - 1, v);
        else
            return q;
    }
    // 最終的に収束した値を返す
    return p;
}

// 集合Sに和がxとなるペアが存在するか
// Sは重複無しソート済み配列とする
int *searchPairSumX(const int *S, int n, int x) {
    int *pair;
    pair = (int *)malloc(2 * sizeof(int));
    if (pair == NULL) {
        printf("can't malloc.\n");
        return NULL;
    }
    int i, j, k, l, partner;
    // xの1/2の整数部分を取得
    // 和がxになるペアのうち, 片方は必ずx/2以下になる
    int h = x / 2;
    // hに最も近い添え字をiに代入
    i = binarySearchRecursive2(S, 0, n - 1, h);
    // xのちょうど半分の値が存在
    if (x % 2 == 0 && S[i] == h) {
        pair[0] = h;
        pair[1] = h;
        return pair;
    }
    // iはh以下で最大の要素の添え字とする
    // S[i]がhより大きければデクリメント
    if (S[i] > h)
        i--;
    //printIntArrayRange(S, 0, i);
    // jはx以下で最大の要素の添え字とする
    j = binarySearchRecursive2(S, i, n - 1, x);
    if (S[j] > x)
        j--;
    //printIntArrayRange(S, i + 1, j);
    // h以下の要素を調べる
    for (k = 0; k <= i; k++) {
        // 足してxになる値を計算
        partner = x - S[k];
        // h以上x以下の範囲で2分探索
        l = binarySearchRecursive(S, i + 1, j, partner);
        // ペアが見つかった
        if (l >= 0) {
            pair[0] = S[k];
            pair[1] = S[l];
            return pair;
        }
    }
    // 見つからなければNULLを返す
    return NULL;
}

int main(void) {
    srand((unsigned)time(NULL));
    int l = 100;
    // 3桁の乱数の配列を作成
    int *sample1 = makeArrayRand3Digits(l);
    int n, i, x;
    int *p;
    //printIntArray(sample1, l);
    // 乱択版クイックソート
    randomizedQuicksort(sample1, 0, l - 1);
    //printIntArray(sample1, l);
    // 重複排除
    n = deduplication(sample1, l);
    // 集合要素数と要素表示
    printf("n = %d\nS = ", n);
    printIntArray(sample1, n);
    // ランダムにxを選ぶ(0 ~ 1998)
    x = rand() % 1999;
    printf("x = %d\n", x);
    // 足してxになるペアを探す
    p = searchPairSumX(sample1, n, x);
    // 見つからなかった
    if (p == NULL) {
        printf("nothing.\n");
    } // 見つかった
    else {
        printf("%d + %d = %d\n", p[0], p[1], x);
    }
    return 0;
}