#ifndef GENETIC02_WIN_H
#define GENETIC02_WIN_H

// othello_win.h を包含するヘッダファイル?
// このファイルを include すればこれも include されるっぽい
#include "othello_win.h"

// よく使う数値
#define SPRM_LEN 10
#define SURVIVE_NUM 10
// 旧GENE_NUM
#define POPULATION 100

// 050: population
// 06: the number of elite
// 00: roulette selection
// 00: uniform crossover
// 00: random mutation
// 005: mutation rate
#define FNF_05006000000005 "prm/sprm050_06_rlt_uni_rd005/sprm050_06_rlt_uni_rd005_g%03d.bin"

// simple parameter
// パレメータの構造体を定義
typedef struct sprm {
    float weight[SPRM_LEN];
} Sprm;

// global variables
extern Board START;
// 正規化された添え字の配列
// 旧 INDEXES
// スペルが気に食わなかった
extern int CORR_TABLE[MASU_NUM];

// functions defined in othello.c
// 全てヘッダファイルから輸入

// これはまた他のファイルから輸入
void quicksortDD(int *A, int *B, int p, int r);

// functions
// パラメータの表示
void showSprm(Sprm pr);

void showFloatArray(float *fa, int n);

int getMin(int a, int b);

int getMinArray(const int *A, int n);

int mirrorLRAd(int src);

// normalize an address
int normalAd(int ad);

// ad: normalized address
int ad2index(int ad);

// -0.5から0.5までの浮動小数点型一様乱数
void randSprm(Sprm *prp);

// 平均化交叉
Sprm makeChildAverageSprm(Sprm mother, Sprm father);

// rateの確率で乱数を返し, それ以外はa, bを半分ずつの確率で返す
float fcrossMFlex(float a, float b, float rate);

// 一様交叉(突然変異率固定)
Sprm makeChildCrossMSprm(Sprm mother, Sprm father);

// convert from an address to the weight index?
// グローバル変数設定
void setCORR_TABLE(void);

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

// ファイル名をフォーマットと世代番号から構築せず, そのまま指定する
int loadSprmFileDirect(const char *fname, Sprm *pra, size_t pra_size);

// ファイルを読み込んで配列に代入
int loadSprmFileFlex(const char *format, int gene_num, Sprm *pra, size_t pra_size);

// n は個体数
// マクロのPOPULATIONは使わずに可変とする
void checkSprmFileFlex(const char *format, int gene_num, int n);

// pythonでパラメータを読み取りたい
// 値を代入するための float のポインタを与える(長さ64の配列)
// ファイル名も指定する
int getTopSprmPy(const char *fnamer, float f_pointer[MASU_NUM]);

// トップ10の平均値を取得(共有ライブラリ用)
// 世代番号でなく, ファイル名で指定する
// ファイル名を世代番号でしていするのはpythonで行なう
void getTop10AveFlexPy(const char *fnamer, float f_pointer[SPRM_LEN]);

// パラメータ毎に平均値を計算する関数
void calcSprmMeans(const Sprm *family, float means[SPRM_LEN], int n);

// パラメータ毎に標準偏差を計算する関数
// 世代の全てのデータを使うことを想定して, 個体数で割ることにする
void calcSprmSD(const Sprm *family, float SD[SPRM_LEN], int n);

// 世代全体の平均値を取得
// 個体数は可変にしたいので長さも引数として与える
int getFamilyMeansPy(const char *fnamer, float f_pointer[SPRM_LEN], int n);

// トップ10の標準偏差を取得(共有ライブラリ用)
// 世代番号でなく, ファイル名で指定する
// ファイル名を世代番号でしていするのはpythonで行なう
// 標本標準偏差?を使うためにデータ数-1で割ることにする
void getTop10SDFlexPy(const char *fnamer, float f_pointer[SPRM_LEN]);

// ある世代全個体の標準偏差を取得
// 個体数以下の数をnに指定することも可能
int getFamilySDPy(const char *fnamer, float f_pointer[SPRM_LEN], int n);

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

// python で使うときにまず実行する
void initPy(void);

// 初期化できているか確認
void showStartPy(void);

// pythonから渡される文字列を表示してみたい
void printStringPy(const char *s);

#endif