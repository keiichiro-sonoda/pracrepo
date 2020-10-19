// this is a source file for debugging
#include <stdlib.h>
#include <time.h>
#include "sort01.h"

int main(void) {
    // use time as seed
    srand((unsigned)time(NULL));
    sortTest();
    return 0;
}