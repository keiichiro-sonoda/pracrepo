#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MASU_NUM 64
#define SPRM_LEN 10

#define printDecimal(x) printf("%d\n", x)

// types
typedef unsigned long int8B;

// board information
typedef struct {
    int8B board[2];
} Board;

// simple parameter
typedef struct {
    float weight[SPRM_LEN];
} Sprm;

// global variables
Board START;
int INDEXES[MASU_NUM];

// functions defined in othello.c
int rotL90DegAd(int src);
void showDecimalArray(const int *ia, int ia_len);
void initBoard(void);
int showBoard(Board b);

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

int getMinArray(const int *A, int n) {
    int i;
    int min = 0x7fffffff;
    for (i = 0; i < n; i++) {
        min = getMin(A[i], min);
    }
    return min;
}

int mirrorLRAd(int src) {
    return (src / 16) * 16 + 14 - src % 16;
}

// normalize an address
int normalAd(int ad) {
    int i, eq_ads[8], adm;
    eq_ads[0] = ad;
    eq_ads[4] = mirrorLRAd(ad);
    for (i = 0; i < 3; i++) {
        eq_ads[i + 1] = rotL90DegAd(eq_ads[i]);
        eq_ads[i + 5] = mirrorLRAd(eq_ads[i + 1]);
    }
    return getMinArray(eq_ads, 8);
}

// ad: normalized address
int ad2index(int ad) {
    if (ad < 16)
        return ad / 2;
    if (ad < 32)
        return ad / 2 - 5;
    if (ad < 48)
        return ad / 2 - 11;
    if (ad < 64)
        return ad / 2 - 18;
    return -1;
}

// -0.5 ~ 0.5
void randSprm(Sprm *prp) {
    int i;
    for (i = 0; i < SPRM_LEN; i++) {
        prp->weight[i] = (float)rand() / RAND_MAX - 0.5;
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
    // set initial board
    initBoard();
    showDecimalArray(INDEXES, MASU_NUM);
    Sprm p1;
    randSprm(&p1);
    showSprm(p1);
    //showDecimalArray(INDEXES, MASU_NUM);
    showBoard(START);
    return 0;
}