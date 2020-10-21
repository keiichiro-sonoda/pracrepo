#ifndef GENETIC02_H
#define GENETIC02_H
// to use MASU_NUM
#include "othello.h"

#define SPRM_LEN 10
#define SURVIVE_NUM 10
#define GENE_NUM 100

// parameter sample
#define SAMP_PRM_NUMS 0.50, -0.20, 0.0, -0.02, -0.25, -0.05, -0.05, 0.0, -0.02, -0.02

// file name formats
#define FNF_SPRM_BASE "prm/simple_prm%03d.bin"

// determine the next board by roulette
// roulette selection (use points directly)
#define FNF_RLTRLT "prm/sprm_rltrlt%03d.bin"

// roulette selection
// subtract the minimum value from all points
#define FNF_RR_SUBMIN "prm/sprm_rr_submin%03d.bin"

// simple parameter
typedef struct sprm{
    float weight[SPRM_LEN];
} Sprm;

// global variables
extern int INDEXES[MASU_NUM];
extern Sprm SAMP_PRM;

// functions

// print a simple parameter
void showSprm(Sprm pr);
// get the smallest value in an array
int getMinArray(const int *A, int n);
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

// assume that the next turn is black
// n: the number of next boards
// use simple parameter
// decide next board by roulette
Board getBoardForBlackSimpleRoulette(Board *next_boards, int n, const Sprm *prp);

// return winner
int oneToOneNormalSprm(const Sprm *spp, const Sprm *gpp);

// return winner
// give a function pointer as an argument
int oneToOneNormalSprmFlex(Board (*decNxt)(Board*, int, const Sprm*), const Sprm *spp, const Sprm *gpp);

// make first file
void makeFirstSprmsFile(void);

// check parameter in a file
// give the file name format and generation number
void checkSprmFile(const char *format, int gene_num);

// use Sprm[100]
// win: +2, draw: +1, lose: 0
void leagueMatchSimpleSprm(Sprm *generation, int *result);

// use Sprm[100]
// win: +2, draw: +1, lose: 0
// give a function pointer to decide the next board
void leagueMatchSprmFlex(Board (*decNxt)(Board*, int, const Sprm*), const Sprm *generation, int *result);

// calculate distance
float distSprm(Sprm p1, Sprm p2);

// calculate means and standard deviation from some parameters
void checkSprmStatistics(const Sprm *pra, int pra_len);

// choose survivors from Sprm[100]
// and show match results
void getSurvivorSprm(Sprm *generation, Sprm *survivors);
// make next generation file
int nextGenerationSprm(int gene_num);

// make next generation file
// give a function to choose survivors
int nextGenerationSprmFlex(void (*getSvr)(const Sprm*, Sprm*), const char *format, int gene_num, int safety);

// loop several times
void nextGenerationSprmLoop(int st, int loop);

// give a function to loop
void nextGenerationSprmLoopFlex(int (*nGene)(int, int), int safety, int st, int loop);

// give a function to loop
void nextGenerationSprmFlexLoopFlex(void (*getSvr)(const Sprm*, Sprm*), int (*nGeneF)(void(), const char*, int, int), const char *format, int safety, int st, int loop);

// make a sample of parameters
void makeSprmSample(void);

// copy the first generation
// give the destination file format
void copyFGFlex(const char *dst_format);

// for debugging
void test1(void);

#endif