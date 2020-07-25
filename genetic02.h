#ifndef GENETIC02_H
#define GENETIC02_H
#include "othello.h"

#define SPRM_LEN 10
#define SURVIVE_NUM 10
#define GENE_NUM 100
#define MOJI "abc"

// macros
#define printDecimal(x) printf("%d\n", x)
#define printFloat(x) printf("%f\n", x)
#define printSize(x) printf("%ld\n", sizeof x)

// simple parameter
typedef struct {
    float weight[SPRM_LEN];
} Sprm;

// global variables
extern int INDEXES[MASU_NUM];

// functions

// print a simple parameters
void showSprm(Sprm pr);
// print an array of floating point number
void showFloatArray(float *fa, int n);
// get the smaller number
int getMin(int a, int b);
// get the smallest value in an array
int getMinArray(const int *A, int n);
// flip horizontal
int mirrorLRAd(int src);
// normalize an address
int normalAd(int ad);
// ad: normalized address
int ad2index(int ad);
// -0.5 ~ 0.5
void randSprm(Sprm *prp);
// calculate average
Sprm makeChildAverageSprm(Sprm mother, Sprm father);
// give mutant rate
float fcrossMFlex(float a, float b, float rate);
// cross parameters?
Sprm makeChildCrossMSprm(Sprm mother, Sprm father);
// convert from an address to the weight index?
void setIndexes(void);
// caluculate point
float evaluationSimple(Board b, Sprm pr);
// assume that the next turn is black
// n: the number of next boards
// use simple parameter
Board getBestBoardForBlackSimple(Board *next_boards, int n, const Sprm *prp);
// return winner
int oneToOneNormalSprm(const Sprm *spp, const Sprm *gpp);
// make first file
void makeFirstSprmsFile(void);
// check parameter in a file
void checkSprmFile(int gene_num);
// use Sprm[100]
// win: +2, draw: +1, lose: 0
void leagueMatchSimpleSprm(Sprm *generation, int *result);
// calculate distance
float distSprm(Sprm p1, Sprm p2);
// choose survivors from Sprm[100]
// and show match results
void getSurvivorSprm(Sprm *generation, Sprm *survivors);
// make next generation file
int nextGenerationSprm(int gene_num);
// loop several times
void nextGenerationSprmLoop(int st, int loop);

#endif