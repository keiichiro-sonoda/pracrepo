#ifndef GENETIC01_h
#define GENETIC01_h

#include "othello_win.h"

#define PI 3.14159265358979323846264338

// number of parameters per generation
#define POPULATION 50
#define PRM1L_LEN 528
#define ELITE_NUM 6
#define MUT_RATE 0.01f

// test file format
// common with genetic02
#define FNF_TEST "prm/test/test%03d.bin"

// L1 : the number of middle layer is 1
// R  : decide next board by roulette
// 050: population
// 06 : the number of elite
// 00 : roulette selection
// 00 : uniform crossover (or none)
// 00 : random mutation
// 001: mutation rate
#define FNF_L1R05006000000001 "prm/l1r050_06_rlt_uni_rd001/l1r050_06_rlt_uni_rd001_g%03d.bin"

// sigmoid function (return float)
#define sigmoidFloat(x) (1.0f / (1 + expf(-10 * (x))))

// return an floating point random number from -0.5 to 0.5
#define randWeight() ((float)rand() / RAND_MAX - 0.5f)

// create an array of random weights
#define randWeightArray(A, n) for (int _ = 0; _ < n; _++) (A)[_] = randWeight()

// choose randomly either a or b
#define alternative(a, b) (rand() % 2 ? (a) : (b))

// uniform crossover (array)
#define uniCrossArray(M, F, C, n) for (int _ = 0; _ < n; _++) (C)[_] = alternative((M)[_], (F)[_])

// MUT_RATE chance to replace with random value
#define randMutArray(C, n) for (int _ = 0; _ < n; _++) if ((float)rand() / RAND_MAX < MUT_RATE) (C)[_] = randWeight()

// 1 middle layer
// including bias
typedef struct prm1L {
    float weight1[8][MASU_NUM + 1];
    float weight2[8];
} Prm1L;

// global variables
// 大域変数
// pythonで用いるパラメータ (中間層1)
extern Prm1L USE_PRM1L;

// functions

void sigmoidFloatP(float *xp);

// black: +1, empty: 0, white: -1
// add an element for bias
void board2arraySymmetryPlus(Board src, int dst[MASU_NUM + 1]);

// convert weight array to Prm1L
void array2Prm1L(float src[PRM1L_LEN], Prm1L *dst);

// convert Prm1L to a weight array
void Prm1L2array(Prm1L *src, float dst[PRM1L_LEN]);

// show parameters (for Prm1L)
void showPrm1L(Prm1L pr);

// create random parameter (for Prm1L)
void randPrm1L(Prm1L *prp);

// calculate point (with Prm1L)
// the more advantageous to black, the higher the score
float evalWithPrm1L(Board b, Prm1L pr);

// assume that the next turn is black
// n: the number of next boards
// use Prm1L
// decide next board by roulette
Board getBoardForBlackPrm1LRlt(Board *next_boards, int n, Prm1L pr);

// python で使うパラメータを設定する
// 読み込みたいファイル名を指定, その中の先頭要素を用いる
// 人数 n はpythonから渡し, 変更可能とする
int setUsePrm1LPy(const char *fname, int n);

// pythonから与えられた盤面に対し, Prm1Lを使って手を選択する
// 今のところルーレット選択を考えている
// 引数にターンを追加
int getActPrm1LPy(int b_info[MASU_NUM], int turn);

// return winner
// give a function pointer to determine the next board (with Prm1L)
// boards are normalized
int oneToOneNPrm1LFlex(Board (*decNxt)(Board*, int, Prm1L), Prm1L spr, Prm1L gpr);

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

// read parameters from a file (Prm1L)
// give a file name directly
int loadPrm1LDirect(const char *fname, Prm1L *pra, size_t pra_size);

// read parameters from a file (Prm1L)
int loadPrm1L(const char *format, int gene_num, Prm1L *pra, size_t pra_size);

// view parematers in a file (Prm1L)
void checkPrm1LFile(const char *format, int gene_num);

#endif