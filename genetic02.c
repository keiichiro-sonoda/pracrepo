#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MASU_NUM 64
#define SPRM_LEN 16

#define printDecimal(x) printf("%d\n", x)

// simple parameter
typedef struct {
    float weight[SPRM_LEN];
} Sprm;

// global variables
int INDEXES[MASU_NUM];

// functions defined in othello.c
int rotL90DegAd(int src);
void showDecimalArray(const int *ia, int ia_len);

// functions

void showSprm(Sprm pr) {
    int i;
    float p;
    for (i = 0; i < MASU_NUM; i++) {
        p = pr.weight[INDEXES[i]];
        printf("%5.2f ", p);
        if (i % 8 == 7) {
            putchar('\n');
        }
    }
}

int getMin(int a, int b) {
    return (a < b ? a : b);
}

// normalize an address
int normalAd(int ad1) {
    int ad2, ad3, ad4, adm;
    ad2 = rotL90DegAd(ad1);
    adm = getMin(ad1, ad2);
    ad3 = rotL90DegAd(ad2);
    adm = getMin(adm, ad3);
    ad4 = rotL90DegAd(ad3);
    return getMin(adm, ad4);
}

// ad: normalized address
int ad2index(int ad) {
    if (ad < 16)
        return ad / 2;
    if (ad < 32)
        return ad / 2 - 2;
    if (ad < 48)
        return ad / 2 - 6;
    if (ad < 64)
        return ad / 2 - 12;
    return -1;
}

// -0.5 ~ 0.5
void randSprm(Sprm *prp) {
    int i;
    float r;
    for (i = 0; i < SPRM_LEN; i++) {
        r = (float)rand() / RAND_MAX - 0.5;
        prp->weight[i] = r;
    }
}

void setIndexes(void) {
    int i, ad;
    for (i = 0; i < MASU_NUM; i++) {
        ad = i * 2;
        INDEXES[i] = ad2index(normalAd(ad));
    }
}

int main(void) {
    // seed reset
    srand((unsigned)time(NULL));
    // set global variable
    setIndexes();
    Sprm p1;
    randSprm(&p1);
    showSprm(p1);
    //showDecimalArray(INDEXES, MASU_NUM);
    return 0;
}