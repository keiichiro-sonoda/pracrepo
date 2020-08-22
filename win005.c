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

int roulette(int *A, int n) {
    int r = rand() % sumInt(A, n);
    int i;
    // 乱数は非負であるからループに必ず入るはず
    for (i = 0; i < n && r >= 0; i++)
        r -= A[i];
    // iは0でないと思う
    return i - 1;
}

int main(void) {
    int n1, t;
    int sample1[] = {8, 4, 1, 2, 0, 3, 9, 6, 7, 5};
    n1 = arrayLength(sample1);
    int result[n1];
    zeros(result, n1);
    t = (unsigned)time(NULL);
    printDecimal(t);
    srand(t);
    printDecimalArray(sample1, n1);
    for (int i = 0; i < 4500000; i++)
        result[roulette(sample1, n1)]++;
    printDecimalArray(result, n1);
    return 0;
}