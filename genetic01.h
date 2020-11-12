#ifndef GENETIC01_h
#define GENETIC01_h

#include "othello.h"

#define PI 3.14159265358979323846264338

// number of parameters per generation
#define POPULATION 100

#define PARAM_NUM 2792

#define PRM1L_LEN 528

#define SURVIVE_NUM 10

// sigmoid function (return float)
#define sigmoidFloat(x) (1.0f / (1 + expf(-10 * (x))))

// return an floating point random number from -0.5 to 0.5
#define randWeight() ((float)rand() / RAND_MAX - 0.5)

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
    float weight1[8][65];
    float weight2[8];
} Prm1L;

typedef struct family{
    Param prms [POPULATION];
} Family;

// global variables

// functions

int sigmoid_pt(float *xp);

// double 0 ~ 1
double drand(void);

// float -0.5 ~ 0.5
float frand(void);

int board2array(Board src, int *dst);

// black: +1, empty: 0, white: -1
// add an element for bias
void board2arraySymmetryPlus(Board src, int dst[MASU_NUM + 1]);

int checkParam(Param pr);

// randomly determine parameter
int paramRand(Param *prp);

// generation initialization
int familyRand(Family *fmp);

// calculate point
// the more advantageous to black, the higher the score
float evaluation(Board b, Param pr);

Board getBestBoard(Board *next_boards, int next_count, int color, Param prm);

// return winnter
// draw: 0, black: 1, white: 2
int oneToOne(Param sente, Param gote);

int leagueMatch(Family fml);

// win: +2, draw: +1, lose: 0
int leagueMatchSimple(Family fml, int *result);

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