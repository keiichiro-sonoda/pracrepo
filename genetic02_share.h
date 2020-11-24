// ヘッダファイルを特定する変数
// 二重インクルード防止
#ifndef GENETIC02_SHARE_H
#define GENETIC02_SHARE_H

// 元ファイルをincludeして再利用しちゃおう
// 被った関数は削除すればいいかな?
// このファイルを include すればついでに include される
// othello.h も包含している
#include "genetic02.h"

// 正規化された添え字の配列
// 旧 INDEXES
// スペルが気に食わなかった
extern int CORR_TABLE[MASU_NUM];

// functions
// パラメータの表示

void showFloatArray(float *fa, int n);

// convert from an address to the weight index?
// グローバル変数設定
void setCORR_TABLE(void);

// make first file
void makeFirstSprmsFile(void);

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

// あるファイルの先頭パラメータ (エリート選択されていると仮定すると, トップパラメータ)
// がランダムAIと対戦したときの (勝ち数, 引き分け数, 負け数) を返したい
// 白黒は引数として与える, トータル処理は python に任せたい
// 個体数と試合数を引数で指定
// 代表者取得関数も使えたが, エラー判定ができないのでやめておく
// 指し手決定関数を決める識別子?も与える
// 0: 固定, 1: ルーレット
int getTopSprmGameRsltVSRandPy(const char *fnamer, int color, int loc_pop, int game_num, int det_func_id, int result[3]);

// use Sprm[100]
// win: +2, draw: +1, lose: 0
void leagueMatchSimpleSprm(Sprm *generation, int *result);

// python で使うときにまず実行する
void initPy(void);

// 初期化できているか確認
void showStartPy(void);

#endif