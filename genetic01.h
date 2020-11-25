#ifndef GENETIC01_H
#define GENETIC01_H

#include "othello.h"

#define PI 3.14159265358979323846264338

// number of parameters per generation
#define POPULATION 50
#define PARAM_NUM 2792
#define PRM1L_LEN 528 // Prm1L の重みの総数
#define ELITE_NUM 6

#ifndef MUT_RATE
#define MUT_RATE 0.05f // 突然変異率
#endif

// ここで活性化関数を指定する
// specify the activation function here
#ifndef ACT_FUNC
#define ACT_FUNC ReLU
#endif

// specify the seed value here
#ifndef SEED
#define SEED 123u
#endif

// バイアスの倍率
// 重みパターンと盤面との乗算結果は-32から32を取り得る (ただしその両端の値を取る確率は極めて低い. と思う)
// 精度は同じchar型を使うが, 幅を広く持たせることにした
// このマクロが定義される以前のものは, これが1であると考えれば良い
#define BIAS_MAG 16

// test file format
// common with genetic02
#define FNF_TEST "prm/test/test%03d.bin"

// L1 : the number of middle layer is 1
// R  : decide next board by roulette
// 050: population
// 06 : the number of elite
// 00 : roulette selection
// 00 : uniform crossover
// 00 : random mutation (or none)
// 001: mutation rate
// default is the step function
#define FNF_L1R05006000000001 "prm/l1r050_06_rlt_uni_rd001/l1r050_06_rlt_uni_rd001_g%03d.bin"

// R  : use ReLU as activation function
#define FNF_L1RR05006000000001 "prm/l1rr050_06_rlt_uni_rd001/l1rr050_06_rlt_uni_rd001_g%03d.bin"
// 突然変異率だけ変更
#define FNF_L1RR05006000000005 "prm/l1rr050_06_rlt_uni_rd005/l1rr050_06_rlt_uni_rd005_g%03d.bin"

// don't use activation function
#define FNF_NON_AF "prm/l1r050_06_rlt_uni_rd001/non_act_func/l1r050_06_rlt_uni_rd001_g%03d.bin"

// return an floating point random number from -0.5 to 0.5
#define randWeight() ((float)rand() / RAND_MAX - 0.5f)

// 圧縮された重みで使うchar型の乱数を作成 (-127から127)
#define randWeightChar() ((char)(rand() % 255))

// create an array of random weights
#define randWeightArray(A, n) for (int _ = 0; _ < n; _++) (A)[_] = randWeight()

// 圧縮されたchar型重みの乱数の配列を作成
// 初期世代ファイルに使う予定
#define randWeightCharArray(A, n) for (int _ = 0; _ < n; _++) (A)[_] = randWeightChar()

// choose randomly either a or b
#define alternative(a, b) (rand() % 2 ? (a) : (b))

// uniform crossover (array)
#define uniCrossArray(M, F, C, n) for (int _ = 0; _ < n; _++) (C)[_] = alternative((M)[_], (F)[_])

// ここでマクロ MUT_RATE を使う
// MUT_RATE chance to replace with random value
#define randMutArray(C, n) for (int _ = 0; _ < n; _++) if ((float)rand() / RAND_MAX < MUT_RATE) (C)[_] = randWeight()

// activation functions
// sigmoid function (return float)
#define sigmoidFloat(x) (1.0f / (1 + expf(-10 * (x))))

// step function
#define step(x) ((x) > 0)

// rectified linear unit (ramp function)
#define ReLU(x) getMax(0, x);

// 重みをchar型に圧縮
// -0.5から0.5を-127から127へ
// char型も一応符号の概念があるっぽい
#define weight2char(w) ((char)((w) * 254))

// 圧縮した数値を重みに戻す
#define char2weight(c) ((float)(c) / 254)

// 圧縮されたchar型配列をfloat型配列に変換
#define char2weightArray(src, dst, n) for (int _ = 0; _ < n; _++) (dst)[_] = char2weight((src)[_])
// float型配列をchar型配列に圧縮
#define weight2charArray(src, dst, n) for (int _ = 0; _ < n; _++) (dst)[_] = weight2char((src)[_])

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

// type of function for selection, crossover and mutation
// ソート済み適応度, ソート済み個体番号, 現世代個体配列, 次世代個体配列
typedef void (*scmFuncPrm1L)(const int*, const int*, const Prm1L*, Prm1L*);

// 大域変数
// global variables

// 圧縮したPrm1L配列の長さ (1要素1byteなので直接サイズとしても使える)
// 最後の要素はソート済みフラグに使う予定
extern int PRM1L_COMP_LEN;

// functions

// Prm1Lを圧縮して保存するGAを使用するときに
// 最初に行う設定関数
// おまけに盤面も初期化してくれる
void initPrm1LComp(void);

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

// play against random
// return winner
// boards are normalized
int Prm1LVSRandomNormal(Prm1L pr, int my_color);

// calculate win rate when playing against random AI
// n: number of games
float calcWinRatePrm1LVSRand(Prm1L pr, int pr_color, int n);

// check win rate when playing against random AI
// n: number of games in each color??
void checkWinRatePrm1LVSRand(Prm1L pr, int n);

int leagueMatch(Family fml);

// win: +2, draw: +1, lose: 0
int leagueMatchSimple(Family fml, int *result);

// with Prm1L[POPULATION]
// win: +2, draw: +1, lose: 0
// give a function pointer to decide the next board
void leagueMatchPrm1LFlex(Board (*decNxt)(Board*, int, Prm1L), const Prm1L *family, int *result);

// write parameters to a file
// give a file name for writing
// be careful of overwriting
int dumpPrm1LDirect(const char *fname, Prm1L *pra, size_t pra_size);

// パラメータを圧縮して書き込む (ファイル名直接指定)
// 上書きに注意
int dumpPrm1LCompDirect(const char *fname, Prm1L *pra);

// 圧縮して書き込み (ファイルフォーマットと世代番号を与える)
int dumpPrm1LComp(const char *format, int generation, Prm1L *pra);

// make first generation file (Prm1L)
// give a file name format
// record all individuals
int makeFGFilePrm1L(const char *format);

// 圧縮バージョンで最初の世代を作成
int makeFGFilePrm1LComp(const char *format);

// read parameters from a file (Prm1L)
// give a file name directly
int loadPrm1LDirect(const char *fname, Prm1L *pra, size_t pra_size);

// read parameters from a file (Prm1L)
int loadPrm1L(const char *format, int gene_num, Prm1L *pra, size_t pra_size);

// ファイル名直接記入バージョン
int loadPrm1LCompDirect(const char *fname, Prm1L *pra);

// 圧縮されたファイルからPrm1Lの配列を取得
int loadPrm1LComp(const char *format, int gene_num, Prm1L *pra);

// load a representative of Prm1L
Prm1L loadRepPrm1L(const char *format, int gene_num, int loc_pop);

// view parematers in a file (Prm1L)
void checkPrm1LFile(const char *format, int gene_num);

int readResultFile(int *result, int r_size, char *fnamel);

// choose survivors
// shallow copy?
int getSurvivor(Family *fmlp, Param *survivors);

float faverage(float f1, float f2);

Param makeChildAverage(Param mother, Param father);

float fcrossM(float f1, float f2);

Param makeChildCrossM(Param mother, Param father);

// uniform crossover (Prm1L)
Prm1L uniCrossPrm1L(Prm1L mother, Prm1L father);

// uniform crossover (Prm1L)
// including random mutation
Prm1L uniCrossRMPrm1L(Prm1L mother, Prm1L father);

// square distance
float sqDist(float f1, float f2);

float paramDistance(Param p1, Param p2);

int nextGeneration(int gene_num);

// roulette selection
// uniform crossover
void rltUniRd(const int *fitness, const int *numbers, const Prm1L *current, Prm1L *next);

// make next generation file
// give a function pointer for selection, crossover and mutation
int nGenePrm1L(scmFuncPrm1L scm, const char *format, int gene_num, int safety);

// 圧縮されたファイルから個体を読み出し, 適応度を評価する
// 適応度降順に個体を並び替え, 同じファイルに書き込む
// 適応度はルーレット選択等に用いるため, 呼び出し元で配列を渡す
int sortPrm1LCompFileByFitness(const char *fname, int *fitness);

// 次の世代のファイルを作る関数 (圧縮バージョン)
// ついでに適応度評価をした現世代のファイルもソートして書き換える (あとで使えそう)
int nGenePrm1LComp(scmFuncPrm1L scm, const char *format, int gene_num, int safety);

// with Prm1L
void nGenePrm1LLoop(scmFuncPrm1L scm, const char *format, int safety, int st, int loop);

// with Prm1L
// use SEED
void nGenePrm1LLoopSeed(scmFuncPrm1L scm, const char *format, int safety, int st, int loop);

#endif