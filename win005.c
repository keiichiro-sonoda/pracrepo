#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define printDecimal(x) printf("%d\n", x)

void printDecimalArray(int *A, int n) {
    putchar('{');
    for (int i = 0; i < n; i++) {
        printf("%d", A[i]);
        if (i < n - 1)
            printf(", ");
    }
    printf("}\n");
}

int sumInt(int *A, int n) {
    int s = 0;
    for (int i = 0; i < n; i++)
        s += A[i];
    return s;
}

int roulette(int *A, int n) {
    return 0;
}

int main(void) {
    int t;
    int n1 = 10;
    int sample1[] = {8, 4, 1, 2, 0, 3, 9, 6, 7, 5};
    t = (unsigned)time(NULL);
    srand(t);
    printDecimal(t);
    printDecimalArray(sample1, n1);
    printDecimal(sumInt(sample1, n1));
    return 0;
}