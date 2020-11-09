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

// マクロは othello_win.h に移動

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

// check parameter in a file
void checkSprmFile(int gene_num);

// pythonでパラメータを読み取りたい
// 書き換えるための float のポインタを与える
// 引数に配列の個数も指定できちゃうの??
// main関数実行前提で考えてた
// 何もしないと CORR_TABLE は設定されない!
void getSprmFilePy(int gene_num, float f_pointer[MASU_NUM]);

// pythonでパラメータを読み取りたい
// 書き換えるための float のポインタを与える(長さ64の配列)
// ファイル名も指定する
void getSprmFileFlexPy(const char *fnamer, float *f_pointer);

// トップだけを観察するとばらつきが大きいため, トップ10の平均値を取ってみる
// 逆に0に近づいてしまうか?
void getTop10AvePy(int gene_num, float f_pointer[SPRM_LEN]);

// トップ10の平均値を取得(共有ライブラリ用)
// 突然変異無しファイルを扱う
// 世代番号, pythonに渡すためのポインタを与える
void getTop10NMAvePy(int gene_num, float f_pointer[SPRM_LEN]);

// トップ10の平均値を取得(共有ライブラリ用)
// 角固定ファイルを扱う(0.5)
// ファイル名フォーマットも引数に与えられるようにしようか?
void getTop10FC1AvePy(int gene_num, float f_pointer[SPRM_LEN]);

// トップ10の平均値を取得(共有ライブラリ用)
// 世代番号でなく, ファイル名で指定する
// ファイル名を世代番号でしていするのはpythonで行なう
void getTop10AveFlexPy(const char *fnamer, float f_pointer[SPRM_LEN]);

// トップ10の標準偏差を取得(共有ライブラリ用)
// 世代番号でなく, ファイル名で指定する
// ファイル名を世代番号でしていするのはpythonで行なう
// 標本標準偏差?を使うためにデータ数-1で割ることにする
void getTop10SDFlexPy(const char *fnamer, float f_pointer[SPRM_LEN]);

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