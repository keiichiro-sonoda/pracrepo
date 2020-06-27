#include <stdio.h>

int main(void) {
    int a[5] = {1, 2, 3, 4, 5};
    int b[5] = {1, 0, -1, -2, -3};
    int c[5];

    for (int i = 0; i < 5; i++) {
        c[i] = a[i] + b[i];
    }

    printf("c = [");
    for (int i = 0; i < 5; i++) {
        printf("%d,", c[i]);
    }

    printf("\b]\n");

    return 0;
}