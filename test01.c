#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "genetic01.h"
#include "sort01.h"

int main(void) {
    srand(124U);
    srand((unsigned)time(NULL));
    Prm1L prm;
    printSize(prm);
    printFloat(randWeight());
    return 0;
}