#ifndef SORT01_WIN_H
#define SORT01_WIN_h

#define expfArray(X, Y, n) for (int _ = 0; _ < n; _++) (Y)[_] = expf((X)[_])

void sortTest(void);

int exchange(int *A, int i, int j);

int partitionDD(int *A, int *B, int p, int r);

// descending order
void quicksortDD(int *A, int *B, int p, int r);

// 浮動小数点型の配列の和を求める
// return the sum of a floating point array
float sumFloat(const float *A, int n);

// ルーレット選択
// 非負の浮動小数点数のみに対応
// roulette selection
// only supports non-negative floating point numbers
int rouletteFloat(const float *A, int n, float s);

#endif