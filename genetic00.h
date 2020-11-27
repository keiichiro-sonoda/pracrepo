#ifndef GENETIC00_h
#define GENETIC00_h

// どうせマクロで指数関数使うんで
#include <math.h>
// オセロはデフォ
// いくつかのライブラリはこれでincludeされる
#include "othello.h"

// パラメータ長
#define PARAM_NUM 2792

#ifndef POPULATION
#define POPULATION 50
#endif

#ifndef ELITE_NUM
#define ELITE_NUM 6
#endif

#ifndef MUT_RATE
#define MUT_RATE 0.05f
#endif

// 0.0から1.0のfloatを返すマクロ
#define randFloat() ((float)rand() / RAND_MAX)
// randFloat() の範囲を-0.5だけずらしたfloatを返す
// return an floating point random number from -0.5 to 0.5
#ifndef randWeight
#define randWeight() (randFloat() - 0.5f)
#endif

// シグモイド関数
// activation functions
// sigmoid function (return float)
#ifndef sigmoidFloat
#define sigmoidFloat(x) (1.0f / (1 + expf(-10 * (x))))
#endif

// 2進数の最下位桁だけで判断
// choose randomly either a or b
#ifndef alternative
#define alternative(a, b) (rand() & 1 ? (a) : (b))
#endif

// ポインタを与えて, 元の数値からシグモイド関数の出力に書き換える
#define sigmoidFloatP(xp) (*(xp) = sigmoidFloat(*(xp)))
// 平均値を計算 (型に依存しないが, 整数等は切り捨てられるため注意)
#define getAve(a, b) (((a) + (b)) / 2)
// 二乗距離を計算
#define sqDist(a, b) square((a) - (b))

// 最初に思いついたでかいパラメータ
// 次はいつ使うんだろうね
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

// パラメータの配列の型をあえて定義する
typedef struct family{
    Param prms [POPULATION];
} Family;

// 盤面をそのまんま配列に変換
void board2array(Board src, int *dst);

// パラメータチェック
void checkParam(Param pr);

// ランダムなパラメータを作る
// randomly determine parameter
void randParam(Param *prp);

// 配列ごと初期化
// generation initialization
void randFamily(Family *fmp);

// 評価値計算
// 活性化関数はシグモイド関数
// calculate point
// the more advantageous to black, the higher the score
float evaluation(Board b, Param pr);

// 評価値計算して最高点だった盤面を返す
Board getBestBoard(Board *next_boards, int next_count, int color, Param prm);

// 初代一対一関数
// return winnter
// draw: 0, black: 1, white: 2
int oneToOne(Param sente, Param gote);

// リーグマッチ関数
// 最初は色とか保存しようとしてたっけ
int leagueMatch(Family fml);

// 勝点を計算するタイプのリーグ戦
// 今はこっちが主流
// win: +2, draw: +1, lose: 0
int leagueMatchSimple(Family fml, int *result);

// 平均化交叉
Param averageCrossover(Param mother, Param father);

// 突然変異付きのどっちか選ぶ関数 (float限定)
float alternativeFM(float f1, float f2);

// 一様交叉
Param uniformCrossoverM(Param mother, Param father);

// パラメータどうしの距離を測る
// ユークリッド距離
float paramDistance(Param p1, Param p2);

// 次の世代へ!
int nextGeneration(int gene_num);

// 何だこれ
int readResultFile(int *result, int r_size, const char *fnamel);

#endif