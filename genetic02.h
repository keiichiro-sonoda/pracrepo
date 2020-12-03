#ifndef GENETIC02_H
#define GENETIC02_H

// to use MASU_NUM
//#include <stdio.h>
//#include <stdlib.h>
#include "othello.h"

#ifndef SEED
#define SEED 123U // シード値
#endif

// 等価でないマスの数
#define SPRM_LEN 10
#define SURVIVE_NUM 10

#ifndef ELITE_NUM
#define ELITE_NUM 6 // エリート数
#endif

#ifndef POPULATION
#define POPULATION 50 // 個体数
#endif

#ifndef MUT_RATE
#define MUT_RATE 0.05f // 突然変異率
#endif

// 指し手固定
// function to determine the nextboard
//#define DET_FUNC getBestBoardForBlackSimple

// 指し手ルーレット選択
#ifndef DET_FUNC
#define DET_FUNC getBoardForBlackSimpleRoulette
#endif

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

// ranking selection
// the probability is 100 - rank (relative)
#define FNF_RRANK "prm/sprm_rrank%03d.bin"

// ranking selection
// the probability is 2 ^ rank (relative)
#define FNF_RRANK_EXP "prm/sprm_rrank_exp/sprm_rrank_exp%03d.bin"

// don't evaluate fitness
// use as a comparison target
#define FNF_NOFIT "prm/sprm_nofit/sprm_nofit%03d.bin"

// test file format
#define FNF_TEST "prm/test/test%03d.bin"

// 050: population
// 06: the number of elite
// 00: roulette selection
// 00: uniform crossover
// 00: random mutation
// 005: mutation rate
#define FNF_05006000000005 "prm/sprm050_06_rlt_uni_rd005/sprm050_06_rlt_uni_rd005_g%03d.bin"

// select the board with the highest evaluation value
#define FNF_D05006000000005 "prm/sprmdef050_06_rlt_uni_rd005/sprmdef050_06_rlt_uni_rd005_g%03d.bin"

// 01: random selection
// 10: average and uniform crossover (once for each parents)
// 忘れちゃいけないのが, 平均化交叉には突然変異入れてないんだよな
// 実質2.5%?
#define FNF_10010011000005 "prm/sprm100_10_rd_au_rd005/sprm100_10_rd_au_rd005_g%03d.bin"
#define FNF_05006011000005 "prm/sprm050_06_rd_au_rd005/sprm050_06_rd_au_rd005_g%03d.bin"

// ルーレット選択, 平均一様, ランダム突変5%
#define FNF_10010001000005 "prm/sprm100_10_rlt_au_rd005/sprm100_10_rlt_au_rd005_g%03d.bin"
#define FNF_05006001000005 "prm/sprm050_06_rlt_au_rd005/sprm050_06_rlt_au_rd005_g%03d.bin"

#define FNF_05006010000005 "prm/sprm050_06_rd_uni_rd005/sprm050_06_rd_uni_rd005_g%03d.bin"

// 02: single point crossover
#define FNF_05006000200005 "prm/sprm050_06_rlt_1p_rd005/sprm050_06_rlt_1p_rd005_g%03d.bin"

// 01: shift mutation (no limit)
#define FNF_05006000001005 "prm/sprm050_06_rlt_uni_sft005/sprm050_06_rlt_uni_sft005_g%03d.bin"

// 02: shift mutation (limited)
#define FNF_05006000002005 "prm/sprm050_06_rlt_uni_sftl005/sprm050_06_rlt_uni_sftl005_g%03d.bin"

// 10: shift mutation (no limit) afte random mutation
// each mutation has a 5% chance of occuring
#define FNF_05006000010005 "prm/sprm050_06_rlt_uni_rdsft005/sprm050_06_rlt_uni_rdsft005_g%03d.bin"

// ランダム選択(01), 一様交叉(00), ランダム突変後制限なしシフト突変(10)
// 指し手ルーレットバージョン
#define FNF_05006010010005 "prm/sprm050_06_rd_uni_rdsft005/sprm050_06_rd_uni_rdsft005_g%03d.bin"
// 指し手固定バージョン
#define FNF_D05006010010005 "prm/sprmdef050_06_rd_uni_rdsft005/sprmdef050_06_rd_uni_rdsft005_g%03d.bin"

// 20: shift mutation (limited) after random mutation
#define FNF_05006000020005 "prm/sprm050_06_rlt_uni_rdsftl005/sprm050_06_rlt_uni_rdsftl005_g%03d.bin"

// 個体数50, エリート6, ランダム突変100%
// ルーレットや一様交叉も0だが, 選択や交叉をしてから突然変異をしたとも考えられる(無理やり)
// やはりアンダースコアで空白を表現した
#define FNF_05006000000100 "prm/sprm050_06___rd100/sprm050_06___rd100_g%03d.bin"

// choose randomly either a or b
#ifndef alternative
#define alternative(a, b) (rand() & 1 ? (a) : (b))
#endif

// Sprm からマスに対応する値を取り出すためのグローバル変数設定
// convert from an address to the weight index?
#define setCORR_TABLE() for (int _ = 0; _ < MASU_NUM; _++) CORR_TABLE[_] = ad2index(normalAd(_ * 2))

// グローバル変数を表示
#define showCORR_TABLE() for (int _ = 0; _ < 8; _++) printDecimalArray(CORR_TABLE + _ * 8, 8)

// サンプルパラメータを作成
// make a sample of parameters
// set global variable "SAMP_PRM"
#define makeSprmSample() do {float spr[] = {SAMP_PRM_NUMS}; for (int _ = 0; _ < SPRM_LEN; _++) SAMP_PRM.weight[_] = spr[_];} while(0)

// パラメータを一行で表示
// print a simple parameter on one line
#define showSprmOneLine(pr) printFloatArray(pr.weight, SPRM_LEN)

// uniform crossover (array)
#ifndef uniCrossArray
#define uniCrossArray(M, F, C, n) for (int _ = 0; _ < n; _++) (C)[_] = alternative((M)[_], (F)[_])
#endif

// 符号無文字型の乱数を返す
// 代入する変数の型に依存?
#define randUchar() (rand() % 256)

// 圧縮された符号無文字を-0.5から0.5までの小数に変換
#define uchar2weight(uc) (((uc) - 127.5f) / 255)

// -0.5から0.5までの小数を符号無文字型に圧縮
// 圧縮精度より小さい値を足せば, 切り捨てられて同じ値になる?
#define weight2uchar(w) (((w) + 0.5000000596f) * 255)

// 符号無文字型の乱数配列を作成
#define randUcharArray(A, n) for (int _ = 0; _ < (n); _++) A[_] = randUchar()

// 符号無文字型配列をfloat型配列に変換
#define uchar2weightArray(src, dst, n) for (int _ = 0; _ < (n); _++) (dst)[_] = uchar2weight((src)[_])

// Sprmの配列を圧縮対応乱数で作成
// n には基本 POPULATION を与えると思うが可変にしておく
// イテレータ変数が被らないように, 各Sprmのポインタを一時変数に代入
#define randSprmArrayComp(pra, n) do {\
    u_char _uca[SPRM_LEN];\
    Sprm *_prp;\
    for (int _ = 0; _ < (n); _++) {\
        _prp = pra + _;\
        randUcharArray(_uca, SPRM_LEN);\
        uchar2weightArray(_uca, _prp->weight, SPRM_LEN);\
    }\
} while (0)

// simple parameter
typedef struct sprm{
    float weight[SPRM_LEN];
} Sprm;

// the type of function pointer to select, crossover and mutation
typedef void (*scmFunc)(const int*, const int*, const Sprm*, Sprm*);

// global variables

// マスとSprmの重みの対応表
extern int CORR_TABLE[MASU_NUM];
// 圧縮版Sprmファイルのサイズ (フラグ付き)
extern const int SPRM_FILE_SIZE_COMP;
extern Sprm SAMP_PRM;

// functions

// グローバル変数等初期化関数
// シード設定も行う
// パラメータも表示
void initSprm(void);

// print a simple parameter
void showSprm(Sprm pr);

// show a part of parameter array
void showFamilyPart(Sprm *pra);

// get the smallest value in an array
int getMinArray(const int *A, int n);
// normalize an address
int normalAd(int ad);
// ad: normalized address
int ad2index(int ad);

// set a parameter to a floating point random number from -0.5 to 0.5
void randSprm(Sprm *prp);

// calculate average
Sprm makeChildAverageSprm(Sprm mother, Sprm father);
// give mutant rate
float fcrossMFlex(float a, float b, float rate);

// uniform crossover
// include mutation
Sprm makeChildCrossMSprm(Sprm mother, Sprm father);

// uniform crossover
// no mutation
Sprm uniCrossSprm(Sprm mother, Sprm father);

// shift the value from 0.05 to 0.1
// no limit
void shiftMutNoLim(Sprm *prp);

// shift the value from 0.05 to 0.10
// limit the value between -0.5 and 0.5
void shiftMutLimited(Sprm *prp);

// mutate with a given probability
// otherwise copy
float copyOrMutation(float x, float mut_rate);

// single point crossover
// give 2 parameters, children's array, and mutation rate
// the number of children is 2
void singlePointCrossover(Sprm mother, Sprm father, Sprm children[2], float mut_rate);

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
// give a function pointer as an argument
int oneToOneNormalSprmFlex(Board (*decNxt)(Board*, int, const Sprm*), const Sprm *spp, const Sprm *gpp);

// play against random
// return winner
// 引数で指し手決定関数を変更可能にした
// pythonで扱うときにマクロをいちいち変更するのが面倒だった
int SprmVSRandomNormal(Board (*decNxt)(Board*, int, const Sprm*), const Sprm *prp, int my_color);

// calculate win rate when playing against random AI
// n: number of games
float calcWinRateSprmVSRand(Sprm pr, int pr_color, int n);

// calculate win rate when playing against random AI
// n: number of games in each color
float calcWinRateSprmVSRandTotal(Sprm pr, int n);

// make first generation file
// give a file name format
// record all individuals!!
int makeFirstGeneFileFlex(const char *format);

// 圧縮版Sprm初期世代ファイルを作成したい
int makeFGFileSprmComp(const char *format);

// ファイル名をそのまま与えてSprmをロード
int loadSprmFileDirect(const char *fname, Sprm *pra, size_t pra_size);

// read parameters from a file
int loadSprmFile(const char *format, int gene_num, Sprm *pra, size_t pra_size);

// load representative of Sprm
Sprm loadRepSprm(const char *format, int gene_num, int loc_pop);

// write parameters to a file
// give a file name for writing
// be careful of overwriting
int dumpSprmFileDirect(const char *fname, Sprm *pra, size_t pra_size);

// check parameter in a file
// give the file name format and generation number
void checkSprmFile(const char *format, int gene_num);

// use Sprm[100]
// win: +2, draw: +1, lose: 0
// give a function pointer to decide the next board
void leagueMatchSprmFlex(Board (*decNxt)(Board*, int, const Sprm*), const Sprm *generation, int *result);

// calculate distance
float distSprm(Sprm p1, Sprm p2);

// calculate means and standard deviation from some parameters
void checkSprmStatistics(const Sprm *pra, int nos);

// choose survivors from Sprm[100]
// and show match results
void getSurvivorSprm(Sprm *generation, Sprm *survivors);

// make next generation file
// give a function to choose survivors
int nextGenerationSprmFlex(void (*getSvr)(const Sprm*, Sprm*), const char *format, int gene_num, int safety);

// give a function to loop
void nextGenerationSprmLoopFlex(int (*nGene)(int, int), int safety, int st, int loop);

// select randomly except for elite selection
// perform averaging and uniform crossing once for each parents
void randAveUni(const int *fitness, const int *numbers, const Sprm *current, Sprm *next);

// make next generation file
// write all individuals to the file
// give a function pointer for selection and crossing
int nGeneSSAFlex(scmFunc selAndCross, const char *format, int gene_num, int safety);

// give a function to loop and file name format
void nGeneSSALoopFlex(int (*nGeneSSA)(const char*, int, int), const char *format, int safety, int st, int loop);

// give a function pointer for selection and crossover
void nGeneSSAFlexLoop(scmFunc selAndCross, const char *format, int safety, int st, int loop);

// give a function pointer for selection and crossover
// use SEED
void nGeneSSAFlexLoopSeed(scmFunc selAndCross, const char *format, int safety, int st, int loop);

// give a function to loop
void nextGenerationSprmFlexLoopFlex(void (*getSvr)(const Sprm*, Sprm*), int (*nGeneF)(void(), const char*, int, int), const char *format, int safety, int st, int loop);

// copy the first generation
// give the destination file format
void copyFGFlex(const char *dst_format);

#endif