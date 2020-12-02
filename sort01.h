#ifndef SORT01_H
#define SORT01_H

// 乱択版クイックソートラッパーマクロ (A はint型配列しか対応しないので注意)
#define randomizedQuicksortAll(A, n) randomizedQuicksort(A, 0, n - 1)

// wrapper function for randomizedQuicksortDD
// マクロにした
#define randomizedQuicksortDDAll(A, B, n) randomizedQuicksortDD(A, B, 0, n - 1)

// float型配列の平均値を求めるマクロ
#define aveFloat(A, n) (sumFloat(A, n) / (n))

// debugging function
void sortTest(void);

// insertion sort
void insertionSort(int *A, int n);

// randomly select 2 integers without duplication
void randIntDoubleDep(int pair[2], int min, int max);

// 重複しないように指定された数だけ範囲内のの整数乱数を取得
void randIntMltDep(int *rd_nums, int n, int min, int max);

// make an array of random numbers
void arrayRandom(int n);

void expArray(const float *X, float *Y, int n);

// make an array of exponents
// int -> float
// give the base of exponents
// the base is only support non-negative number
void expArrayIFFlex(const int *X, float *Y, int n, float base);

// return the sum of an integer array
int sumInt(const int *A, int n);

// return the sum of a floating point array
float sumFloat(const float *A, int n);

// delete the element of the specified index
// the size of the array doesn't change
void delInt(int *A, int n, int index);

// delete the element of the specified index
// the size of the array doesn't change
void delFloat(float *A, int n, int index);

// roulette selection
// only supports non-negative integers
// return an index
int rouletteInt(const int *A, int n, int s);

// roulette selection
// only supports non-negative floating point numbers
int rouletteFloat(const float *A, int n, float s);

// fix the indices misalignment
void fixIndices(int *A, int n);

// choose some elements with roulette
// don't allow duplication
// only supports non-negative integers
void rouletteIntMltDep(const int *A, int A_len, int *rslt, int rslt_len);

// choose some elements with roulette
// don't allow duplication
void rouletteFloatMltDep(const float *A, int A_len, int *rslt, int rslt_len);

// check if rouletteFloat workes as expected
void rouletteFloatTest(const float *A, int n);

// exchange A[i] and A[j]
void exchange(int *A, int i, int j);

// exchange A[i] and A[j]
// also exchange B[i] and B[j]
void exchangeD(int *A, int *B, int i, int j);

// 乱択版パーティション (昇順)
int randomizedPartition(int *A, int p, int r);

// randomized partition
// A: array to compare
// B: sorted by A
int randomizedPartitionDD(int *A, int *B, int p, int r);

int partitionDD(int *A, int *B, int p, int r);
// descending order
void quicksortDD(int *A, int *B, int p, int r);

// 乱択版クイックソート (昇順)
void randomizedQuicksort(int *A, int p, int r);

// randomized quicksort
void randomizedQuicksortDD(int *A, int *B, int p, int r);

#endif