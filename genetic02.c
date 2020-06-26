#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

#define MASU_NUM 64
#define SPRM_LEN 10

#define printDecimal(x) printf("%d\n", x)
#define printFloat(x) printf("%f\n", x)

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
void board2arraySymmetry(Board src, int *dst);

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

// convert from an address to the weight index?
void setIndexes(void) {
    int i, ad;
    for (i = 0; i < MASU_NUM; i++) {
        ad = i * 2;
        INDEXES[i] = ad2index(normalAd(ad));
    }
}

// caluculate point
float evaluationSimple(Board b, Sprm pr) {
    float pt = 0;
    int i;
    int ba[MASU_NUM];
    // Board -> int array
    board2arraySymmetry(b, ba);
    showDecimalArray(ba, MASU_NUM);
    for (i = 0; i < MASU_NUM; i++) {
        // calculate inner product
        pt += ba[i] * pr.weight[INDEXES[i]];
    }
    return pt;
}

// assume that the next turn is black
// n: the number of next boards
// use simple parameter
Board getBestBoardForBlackSprm(Board *next_boards, int n, const Sprm *prp) {
    float mx_point = -FLT_MAX;
    float t_point;
    int i;
    Board best_board;
    for (i = 0; i < n; i++) {
        t_point = evaluationSimple(next_boards[i], *prp);
        if (mx_point < t_point) {
            // update
            mx_point = t_point;
            best_board = next_boards[i];
        }
    }
    printf("%5.2f\n", mx_point);
    return best_board;
}

int main(void) {
    // seed reset
    srand((unsigned)time(NULL));
    // set global variable
    setIndexes();
    // set initial board
    initBoard();

    Sprm p1;
    randSprm(&p1);
    showSprm(p1);
    // afte one action
    Board sample;
    sample = START;
    sample.board[1] = 0x0000000000000150;
    showBoard(sample);
    printFloat(evaluationSimple(sample, p1));
    return 0;
}