#ifndef ARRAY_H // 二重でincludeされることを防ぐ
#define ARRAY_H
#define printDecimal(x) printf("%d\n", x)
#define printFloat(x) printf("%f\n", x)

void printDecimalArray(const int *A, int n);
void printFloatArray(const float *A, int n);
void range(int *A, int n, int start, int step);
void init(int *A, int n, int num);
void zeros(int *A, int n);
void randArray(int *A, int n);
void expArray(float *X, float *Y, int n);
int sum(int *A, int n);
float sumFloat(float *A, int n);

#endif