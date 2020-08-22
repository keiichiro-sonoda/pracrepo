#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define printDecimal(x) printf("%d\n", x)

int main(void) {
    int r;
    srand((unsigned)time(NULL));
    r = rand();
    printDecimal(r);
    return 0;
}