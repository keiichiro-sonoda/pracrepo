#include <stdio.h>

int main(void) {
    int size = 10;
    int i, a[size], b[size], c[size];

    for (i = 0; i < size; i++) {
        a[i] = i + 1;
        b[i] = 1 - i;
    }
    
    for (i = 0; i < size; i++) {
        c[i] = a[i] + b[i];
    }

    printf("c = [");
    for (i = 0; i < size; i++) {
        printf("%d,", c[i]);
    }

    printf("\b]\n");

    return 0;
}