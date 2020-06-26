#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

#define NEXT_MAX 32
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
void swapNormalizeBoard(Board *bp);
int nextBoardNormal2(Board b, Board *next_boards, int *koma_count);

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

void showFloatArray(float *fa, int n) {
    int i;
    putchar('{');
    for (i = 0; i < n; i++) {
        printf("%5.2f", fa[i]);
        if (i < n - 1) {
            printf(", ");
        }
    }
    printf("}\n");
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

// calculate average
Sprm makeChildAverageSprm(Sprm mother, Sprm father) {
    Sprm child;
    int i;
    for (i = 0; i < SPRM_LEN; i++) {
        child.weight[i] = (mother.weight[i] + father.weight[i]) / 2;
    }
    return child;
}

// give mutant rate
float fcrossMFlex(float a, float b, float rate) {
    // 0.0 ~ 1.0
    float r = (float)rand() / RAND_MAX;
    // mutant!
    if (r <= rate) return (float)rand() / RAND_MAX - 0.5;
    r = (float)rand() / RAND_MAX;
    // 50 : 50
    if (r < 0.5) return a;
    return b;
}

Sprm makeChildCrossMSprm(Sprm mother, Sprm father) {
    Sprm child;
    int i;
    for (i = 0; i < SPRM_LEN; i++) {
        child.weight[i] = fcrossMFlex(mother.weight[i], father.weight[i], 0.05);
    }
    return child;
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
    //showDecimalArray(ba, MASU_NUM);
    for (i = 0; i < MASU_NUM; i++) {
        // calculate inner product
        pt += ba[i] * pr.weight[INDEXES[i]];
    }
    return pt;
}

// assume that the next turn is black
// n: the number of next boards
// use simple parameter
Board getBestBoardForBlackSimple(Board *next_boards, int n, const Sprm *prp) {
    float mx_point = -FLT_MAX;
    float t_point;
    int i;
    Board best_board;
    for (i = 0; i < n; i++) {
        // sign inversion!!
        t_point = -evaluationSimple(next_boards[i], *prp);
        if (mx_point < t_point) {
            // update
            mx_point = t_point;
            best_board = next_boards[i];
        }
    }
    //printf("%5.2f\n", mx_point);
    return best_board;
}

// return winner
int oneToOneNormalSprm(const Sprm *spp, const Sprm *gpp) {
    Board nba[NEXT_MAX];
    int kc[3];
    int pass = 0;
    int n, dif;
    // set turn
    int turn = 0b01;
    // set initial board
    Board main_board = START;
    while (1) {
        // calculate next
        n = nextBoardNormal2(main_board, nba, kc);
        showBoard(main_board);
        // can't put a piece anywhere
        if (n == 0) {
            // can't do anything one another
            if (pass) {
                printf("end\n");
                break;
            }
            // pass
            printf("pass\n");
            swapNormalizeBoard(&main_board);
            turn ^= 0b11;
            pass = 1;
            continue;
        }
        pass = 0;
        // determine a next board
        // black (first)
        if (turn == 0b01) {
            printf("black\n");
            main_board = getBestBoardForBlackSimple(nba, n, spp);
        } // white (second)
        else {
            printf("white\n");
            main_board = getBestBoardForBlackSimple(nba, n, gpp);
        }
        // switch turn
        turn ^= 0b11;
    }
    // difference between black and white
    dif = kc[1] - kc[2];
    printDecimal(dif);
    if (dif > 0) return turn;
    if (dif < 0) return turn ^ 0b11;
    // draw
    return 0;
}

int main(void) {
    // seed reset
    srand((unsigned)time(NULL));
    // set global variable
    setIndexes();
    // set initial board
    initBoard();

    int res;
    Sprm p1, p2, p3;
    // random paramater
    randSprm(&p1);
    randSprm(&p2);
    //showSprm(p1);
    showFloatArray(p1.weight, SPRM_LEN);
    showFloatArray(p2.weight, SPRM_LEN);
    // afte one action
    //res = oneToOneNormalSprm(&p1, &p2);
    p3 = makeChildAverageSprm(p1, p2);
    showFloatArray(p3.weight, SPRM_LEN);
    return 0;
}