#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define kugiri() printf("---------------------------------\n")
#define printSize(x) printf("%ld\n", sizeof x)
#define printDecimal(x) printf("%d\n", x)

void printIntArray(const int *A, int n) {
    int i;
    for (i = 0; i < n; i++)
        printf("%d ", A[i]);
    putchar('\n');
    return;
}

void printBin(int x) {
    int A[64];
    int i, j;
    for (i = 0; x != 0; i++) {
        A[i] = x & 1;
        x >>= 1;
    }
    for (j = i - 1; j >= 0; j--)
        printf("%d", A[j]);
    putchar('\n');
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

int main(void) {
    srand((unsigned)time(NULL));
    int l = 1000;
    int *sample1 = makeArrayRand3Digits(l);
    int sl;
    //printIntArray(sample1, l);
    randomizedQuicksort(sample1, 0, l - 1);
    printIntArray(sample1, l);
    sl = deduplication(sample1, l);
    printDecimal(sl);
    printIntArray(sample1, sl);
    return 0;
}