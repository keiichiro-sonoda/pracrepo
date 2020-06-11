#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

int main(void) {
    srand((unsigned)time(NULL));
    int r;
    r = rand();
    printf("%ld\n", sizeof r);
    printf("%d\n", r);
    printf("%x\n", r);
    printBin(r);
    return 0;
}