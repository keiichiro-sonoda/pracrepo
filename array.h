#ifndef ARRAY_H // 二重でincludeされることを防ぐ
#define ARRAY_H

void printDecimalArray(const int *A, int n);
void range(int *A, int n, int start, int step);
void init(int *A, int n, int num);
void zeros(int *A, int n);

#endif