#ifndef SORT01_H
#define SORT01_H

// ダブってもいいや
#include "math.h"

// tmp に A[i] を退避させ, A[i] と A[j] を交換
#define _exchange(A, i, j, tmp) (tmp = (A)[i], (A)[i] = (A)[j], (A)[j] = tmp)

// 配列 A の添字 i と j の要素を交換
// exchange A[i] and A[j]
#define exchange(type, A, i, j) do {type _; _exchange(A, i, j, _);} while (0)

// 2つの配列で, それぞれ添字 i と j の要素を交換
// exchange A[i] and A[j]
// also exchange B[i] and B[j]
#define exchangeD(type, A, B, i, j) do {type _; _exchange(A, i, j, _); _exchange(B, i, j, _);} while (0)

// 降順, Aの値を基準にBも一緒にクイックソート
// ラッパー関数型マクロ
#define quicksortDDAll(A, B, n) quicksortDD(A, B, 0, (n) - 1)

// 乱択版クイックソートラッパーマクロ (A はint型配列しか対応しないので注意)
#define randomizedQuicksortAll(A, n) randomizedQuicksort(A, 0, n - 1)

// wrapper function for randomizedQuicksortDD
// マクロにした
#define randomizedQuicksortDDAll(A, B, n) randomizedQuicksortDD(A, B, 0, (n) - 1)

// ある添字の要素を削除する
//左詰めをして, あたかも配列が短くなったように見せかける
#define delElement(A, n, index) for (int _ = (index) + 1; _ < (n); _++) (A)[_ - 1] = (A)[_]

// sum に配列 A の合計を代入する
// sum は 0 で初期化されている前提
#define addAll(A, n, sum) for (int _ = 0; _ < (n); _++) (sum) += (A)[_]

// float型配列の平均値を求めるマクロ
#define aveFloat(A, n) (sumFloat(A, n) / (n))

// 配列 X の指数関数の出力を Y に代入
// X の型は不問, Y は double 型推奨
// 底は非負
#define powArray(X, Y, n, base) for (int _ = 0; _ <(n); _++) (Y)[_] = pow((base), (X)[_])

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

// double 型の配列の和を返す
double sumDouble(const double *A, int n);

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

// 合計値も計算するバージョン
// int 型配列 (非負), 配列長
int rouletteIntCalcSum(const int*, int);

// roulette selection
// only supports non-negative floating point numbers
int rouletteFloat(const float *A, int n, float s);

// double 型の配列でルーレット選択
// 値は非負であること前提
int rouletteDouble(const double*, int, double);

// fix the indices misalignment
void fixIndices(int *A, int n);

// choose some elements with roulette
// don't allow duplication
// only supports non-negative integers
void rouletteIntMltDep(const int *A, int A_len, int *rslt, int rslt_len);

// choose some elements with roulette
// don't allow duplication
void rouletteFloatMltDep(const float *A, int A_len, int *rslt, int rslt_len);

// ルーレット選択で重複なしで要素を選択
// 選ばれた要素は選択肢から排除
void rouletteDoubleMltDep(const double *A, int A_len, int *rslt, int rslt_len);

// check if rouletteFloat workes as expected
void rouletteFloatTest(const float *A, int n);

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