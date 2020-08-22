#include <stdio.h>
#include <stdlib.h>

#define printDecimal(x) printf("%d\n", x)

int main(void) {
    int r;
    r = rand();
    printDecimal(r);
    return 0;
}