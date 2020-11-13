#ifndef GENETIC01_h
#define GENETIC01_h

#include "othello.h"

#define PI 3.14159265358979323846264338

// number of parameters per generation
#define POPULATION 100
#define PARAM_NUM 2792
#define PRM1L_LEN 528
#define SURVIVE_NUM 10

// test file format
#define FNF_TEST "prm/test/test%03d.bin"

// sigmoid function (return float)
#define sigmoidFloat(x) (1.0f / (1 + expf(-10 * (x))))

// return an floating point random number from -0.5 to 0.5
#define randWeight() ((float)rand() / RAND_MAX - 0.5)

// create an array of random weights
#define randWeightArray(A, n) for (int _ = 0; _ < n; _++) (A)[_] = randWeight()

// types

typedef struct param {
    float weight1[32][64];
    float weight2[16][32];
    float weight3[8][16];
    float weight4[4][8];
    float weight5[2][4];
    float weight6[2];
    float bias1[32];
    float bias2[16];
    float bias3[8];
    float bias4[4];
    float bias5[2];
} Param;

// 1 middle layer
// including bias
typedef struct prm1L {
    float weight1[8][MASU_NUM + 1];
    float weight2[8];
} Prm1L;

typedef struct family{
    Param prms [POPULATION];
} Family;

// global variables

// functions

void sigmoidFloatP(float *xp);

int board2array(Board src, int *dst);

// black: +1, empty: 0, white: -1
// add an element for bias
void board2arraySymmetryPlus(Board src, int dst[MASU_NUM + 1]);

// convert weight array to Prm1L
void array2Prm1L(float src[PRM1L_LEN], Prm1L *dst);

// convert Prm1L to a weight array
void Prm1L2array(Prm1L *src, float dst[PRM1L_LEN]);

int checkParam(Param pr);

// show parameters (for Prm1L)
void showPrm1L(Prm1L pr);

// randomly determine parameter
int paramRand(Param *prp);

// create random parameter (for Prm1L)
void randPrm1L(Prm1L *prp);

// generation initialization
int familyRand(Family *fmp);

// calculate point
// the more advantageous to black, the higher the score
float evaluation(Board b, Param pr);

// calculate point (with Prm1L)
// the more advantageous to black, the higher the score
float evalWithPrm1L(Board b, Prm1L pr);

Board getBestBoard(Board *next_boards, int next_count, int color, Param prm);

// assume that the next turn is black
// n: the number of next boards
// use Prm1L
// decide next board by roulette
Board getBoardForBlackPrm1LRlt(Board *next_boards, int n, Prm1L pr);

// return winnter
// draw: 0, black: 1, white: 2
int oneToOne(Param sente, Param gote);

// return winner
// give a function pointer to determine the next board (with Prm1L)
// boards are normalized
int oneToOneNPrm1LFlex(Board (*decNxt)(Board*, int, Prm1L), Prm1L spr, Prm1L gpr);

int leagueMatch(Family fml);

// win: +2, draw: +1, lose: 0
int leagueMatchSimple(Family fml, int *result);

// with Prm1L[POPULATION]
// win: +2, draw: +1, lose: 0
// give a function pointer to decide the next board
void leagueMatchPrm1LFlex(Board (*decNxt)(Board*, int, Prm1L), const Prm1L *family, int *result);

// warning before overwriting
// importet from genetic02.c
int warnOverwriting(const char *fname);

// write parameters to a file
// give a file name for writing
// be careful of overwriting
int dumpPrm1LDirect(const char *fname, Prm1L *pra, size_t pra_size);

// make first generation file (Prm1L)
// give a file name format
// record all individuals
int makeFGFilePrm1L(const char *format);

int readResultFile(int *result, int r_size, char *fnamel);

// choose survivors
// shallow copy?
int getSurvivor(Family *fmlp, Param *survivors);

float faverage(float f1, float f2);

Param makeChildAverage(Param mother, Param father);

float fcrossM(float f1, float f2);

Param makeChildCrossM(Param mother, Param father);

// square distance
float sqDist(float f1, float f2);

float paramDistance(Param p1, Param p2);

int nextGeneration(int gene_num);

#endif