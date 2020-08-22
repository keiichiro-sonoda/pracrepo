#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define printDecimal(x) printf("%d\n", x)

int main(void) {
    int t;
    t = (unsigned)time(NULL);
    srand(t);
    printDecimal(t);
    for (int i = 0; i < 3; i++) {
        printDecimal(rand());
    }
    return 0;
}