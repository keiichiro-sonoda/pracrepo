// ヘッダファイルを特定する変数
// 二重インクルード防止
#ifndef GENETIC02_SHARE_H
#define GENETIC02_SHARE_H

// 元ファイルをincludeして再利用しちゃおう
// 被った関数は削除すればいいかな?
// このファイルを include すればついでに include される
// othello.h も包含している
#include "genetic02.h"

// Sprmファイルを圧縮かどうかのフラグを与えて柔軟に読み込ませる?
// エラーなら呼び出し元で戻る
// サイズは個体数から計算
#define loadSprmFileDirectFlexExit(fname, pra, n, compressed) do {\
    int e;\
    size_t _pra_size = sizeof(Sprm) * (n);\
    switch (compressed) {\
        case 0 : loadFileDirectExit(fnamer, family, _pra_size);\
        default: e = loadSprmFileCompDirect(fnamer, family, n);\
    }\
    if (e < 0) return -1;\
} while (0)

// functions
// パラメータの表示

// Sprm のファイルフォーマットを自動生成する関数
// Python 用
// ただし研究対象ということで等比数列ランキング選択に限る
// 書き込み対象文字列, サイズ, 個体数, エリート数, 選択方法, シード値, 公比 (対数かどうかは sel_id 依存)
int makeSprmFileFormatRankGeoProgPy(char*, int, int, int, int, int, double);

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

// 世代全体の平均値を取得
// 個体数は可変にしたいので長さも引数として与える
// 無効なファイル名が渡されたときの処理も追加
// pythonでいちいち関数増やすのもだるいので, 圧縮版も兼ねてしまおう
// 引数を一つ増やして識別したほうが簡単そうだ…多分
// compressed が0なら非圧縮, 0以外なら圧縮
int getFamilyMeansPy(const char*, float*, int, int);

// トップ10の標準偏差を取得(共有ライブラリ用)
// 世代番号でなく, ファイル名で指定する
// ファイル名を世代番号でしていするのはpythonで行なう
// 標本標準偏差?を使うためにデータ数-1で割ることにする
void getTop10SDFlexPy(const char *fnamer, float f_pointer[SPRM_LEN]);

// ある世代全個体の標準偏差を取得
// 個体数以下の数をnに指定することも可能
// 圧縮版か否かを判別する引数を追加
int getFamilySDPy(const char*, float*, int, int);

// あるファイルの先頭パラメータ (エリート選択されていると仮定すると, トップパラメータ)
// がランダムAIと対戦したときの (勝ち数, 引き分け数, 負け数) を返したい
// 白黒は引数として与える, トータル処理は python に任せたい
// 個体数と試合数を引数で指定
// 代表者取得関数も使えたが, エラー判定ができないのでやめておく
// 指し手決定関数を決める識別子?も与える
// 0: 固定, 1: ルーレット
int getTopSprmGameRsltVSRandPy(const char *fnamer, int color, int loc_pop, int game_num, int det_func_id, int result[3]);

// python で使うときにまず実行する
void initPy(void);

// シード値設定
void setSeedPy(unsigned int seed);

// 乱数チェック
void checkRandPy(void);

// 初期化できているか確認
void showStartPy(void);

// 適応度ファイルを Python から読み取る
// fitness には loc_pop 以上の領域を確保すること
int getFitnessPy(const char*, int*, int);

#endif