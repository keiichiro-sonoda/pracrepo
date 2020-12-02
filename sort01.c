#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "sort01.h"
#include "othello.h"

// for debugging
void sortTest(void) {
    srand(123u);
    //srand((unsigned)time(NULL));
    int sample1[] = {11, 5, 6, 8, 1, 2, 10, 3, 4, 2, 10, 9, 7, 20, 0, -2, -1};
    float sample2[] = {-100.0, -1.0, -0.5, -0.2, 0.0, 0.1, 0.3, 1.0, 2.5, 4.0, 80.0};
    int l1 = arrayLength(sample1);
    int l2 = arrayLength(sample2);
    randomizedQuicksortAll(sample1, l1);
    printDecimalArray(sample1, l1);
    printFloat(aveFloat(sample2, l2));
}

// insertion sort
void insertionSort(int *A, int n) {
    int i, j, key;
    for (j = 1; j < n; j++) {
        key = A[j];
        // A[j]をソート済みの列A[1..j-1]に挿入する.
        for (i = j - 1; i >= 0 && A[i] > key; i--)
            A[i + 1] = A[i];
        A[i + 1] = key;
    }
}

// 重複しないように指定された範囲の整数乱数を2つ取得
// randomly select 2 integers without duplication
void randIntDoubleDep(int pair[2], int min, int max) {
    pair[0] = randIntRange(min, max);
    // 最大値を1減らす (取り得る範囲を小さくする)
    pair[1] = randIntRange(min, max - 1);
    // 1回目の乱数以上なら1を足して調整
    if (pair[1] >= pair[0])
        pair[1]++;
}

// make an array of exponents
// to make the numbers positive
void expArray(const float *X, float *Y, int n) {
    for (int i = 0; i < n; i++)
        Y[i] = (float)exp(X[i]);
}

// make an array of exponents
// int -> float
// give the base of exponents
// the base is only support non-negative number
void expArrayIFFlex(const int *X, float *Y, int n, float base) {
    for (int i = 0; i < n; i++)
        Y[i] = powf(base, (float)X[i]);
}

// return the sum of an integer array
int sumInt(const int *A, int n) {
    int s = 0;
    for (int i = 0; i < n; i++)
        s += A[i];
    return s;
}

// return the sum of a floating point array
float sumFloat(const float *A, int n) {
    float s = 0.0f;
    for (int i = 0; i < n; i++)
        s += A[i];
    return s;
}

// delete the element of the specified index
// the size of the array doesn't change
void delInt(int *A, int n, int index) {
    for (int i = index + 1; i < n; i++)
        A[i - 1] = A[i];
}

// delete the element of the specified index
// the size of the array doesn't change
void delFloat(float *A, int n, int index) {
    for (int i = index + 1; i < n; i++)
        A[i - 1] = A[i];
}

// roulette selection
// only supports non-negative integers
// return an index
int rouletteInt(const int *A, int n, int s) {
    // avoid dividing by 0
    if (s == 0) return 0;
    int r = rand() % s;
    int i;
    for (i = 0; i < n - 1; i++) {
        r -= A[i];
        if (r < 0)
            return i;
    }
    return i;
}

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

// fix the indices misalignment
void fixIndices(int *A, int n) {
    for (int i = n - 2; i >= 0; i--)
        for (int j = i + 1; j < n; j++)
            if (A[i] <= A[j])
                A[j]++;
}

// choose some elements with roulette
// don't allow duplication
// only supports non-negative integers
void rouletteIntMltDep(const int *A, int A_len, int *rslt, int rslt_len) {
    zeros(rslt, rslt_len);
    int now[A_len];
    copyArray(A, now, A_len);
    int now_len;
    int s = sumInt(A, A_len);
    for (int i = 0; i < rslt_len; i++) {
        now_len = A_len - i;
        //printDecimalArray(now, now_len);
        //printDecimal(s);
        rslt[i] = rouletteInt(now, now_len, s);
        s -= now[rslt[i]];
        // delete the selected element
        delInt(now, now_len, rslt[i]);
    }
    // fix the results
    fixIndices(rslt, rslt_len);
}

// choose some elements with roulette
// don't allow duplication
void rouletteFloatMltDep(const float *A, int A_len, int *rslt, int rslt_len) {
    // initialize
    zeros(rslt, rslt_len);
    float now[A_len];
    copyArray(A, now, A_len);
    int now_len;
    float s = sumFloat(A, A_len);
    for (int i = 0; i < rslt_len; i++) {
        now_len = A_len - i;
        rslt[i] = rouletteFloat(now, now_len, s);
        s -= now[rslt[i]];
        // delete the selected element
        delFloat(now, now_len, rslt[i]);
    }
    // fix the results
    fixIndices(rslt, rslt_len);
}

// check if rouletteFloat workes as expected
void rouletteFloatTest(const float *A, int n) {
    float s = sumFloat(A, n);
    int loop = s * 10000;
    printDecimal(loop);
    int results[n];
    // initialize
    zeros(results, n);
    for (int i = 0; i < loop; i++)
        results[rouletteFloat(A, n, s)]++;
    printDecimalArray(results, n);
}

// exchange A[i] and A[j]
void exchange(int *A, int i, int j) {
    int t = A[i];
    A[i] = A[j];
    A[j] = t;
}

// exchange A[i] and A[j]
// also exchange B[i] and B[j]
void exchangeD(int *A, int *B, int i, int j) {
    int t = A[i];
    A[i] = A[j];
    A[j] = t;
    t = B[i];
    B[i] = B[j];
    B[j] = t;
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

// 乱択版パーティション (昇順)
int randomizedPartition(int *A, int p, int r) {
    exchange(A, randIntRange(p, r), r);
    int x = A[r];
    int i = p;
    for (int j = p; j < r; j++) {
        if (A[j] < x) {
            exchange(A, i, j);
            i++;
        }
    }
    exchange(A, i, r);
    return i;
}

// randomized partition
// A: array to compare
// B: sorted by A
int randomizedPartitionDD(int *A, int *B, int p, int r) {
    // 分割の基準となる値はランダムに決定する
    int i = randIntRange(p, r);
    // randomized!
    exchangeD(A, B, i, r);
    int x = A[r];
    i = p;
    for (int j = p; j < r; j++) {
        if (A[j] > x) {
            exchangeD(A, B, i, j);
            i++;
        }
    }
    exchangeD(A, B, i, r);
    return i;
}

// descending order
// also rearrange the indices
void quicksortDD(int *A, int *B, int p, int r) {
    if (p < r) {
        int q = partitionDD(A, B, p, r);
        quicksortDD(A, B, p, q - 1);
        quicksortDD(A, B, q + 1, r);
    }
}

// 乱択版クイックソート (昇順)
void randomizedQuicksort(int *A, int p, int r) {
    if (p < r) {
        int q = randomizedPartition(A, p, r);
        randomizedQuicksort(A, p, q - 1);
        randomizedQuicksort(A, q + 1, r);
    }
}

// randomized quicksort
void randomizedQuicksortDD(int *A, int *B, int p, int r) {
    if (p < r) {
        int q = randomizedPartitionDD(A, B, p, r);
        randomizedQuicksortDD(A, B, p, q - 1);
        randomizedQuicksortDD(A, B, q + 1, r);
    }
}

// wrapper function for randomizedQuicksortDD
void randomizedQuicksortDDAll(int *A, int *B, int n) {
    randomizedQuicksortDD(A, B, 0, n - 1);
}