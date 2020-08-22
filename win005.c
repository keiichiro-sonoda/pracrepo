#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define printDecimal(x) printf("%d\n", x)

int main(void) {
    int t, r;
    t = (unsigned)time(NULL);
    srand(t);
    r = rand();
    printDecimal(t);
    printDecimal(r);
    return 0;
}