// fix the corner evaluation
#include <stdio.h>
#include "genetic02.h"

// copy the first generation
// and set the corner evaluation 0.5
void copyFGFixCorner1(void) {
    FILE *fp;
    // file name for reading
    char fnamer[] = "prm/simple_prm000.bin";
    if ((fp = fopen(fnamer, "rb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fnamer);
        return;
    }
    // opened!
    Sprm pa[SURVIVE_NUM];
    fread(pa, sizeof pa, 1, fp);
    fclose(fp);
    // check parameters
    for (int i = 0; i < SURVIVE_NUM; i++) {
        printFloatArray(pa[i].weight, SPRM_LEN);
        // fix the corner
        pa[i].weight[0] = 0.5;
    }
    printString("fix!!");
    // reconfirmation
    for (int i = 0; i < SURVIVE_NUM; i++) {
        printFloatArray(pa[i].weight, SPRM_LEN);
    }
    // file name for writing
    char fnamew[] = "prm/sprm_corner0.5_000.bin";
    // open a file to write (or make a file)
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fnamew);
        return;
    }
    // opened!
    fwrite(pa, sizeof pa, 1, fp);
    // close
    fclose(fp);
}

int main(void) {
    setIndexes();
    copyFGFixCorner1();
    return 0;
}