// pythonのctypesから呼び出す共有ライブラリを作る
// ubuntu で python を実行することを考慮すると様々な問題が発生
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <math.h>

// この中に genetic02.h と othello.h が包含されている
#include "genetic02_share.h"
#include "sort01.h"

// global variables

// functions

// make first file
void makeFirstSprmsFile(void) {
    FILE *fp;
    char fnamew[] = "prm/simple_prm000.bin";
    int i;
    Sprm pra[10];
    // open a file to write (or make a file)
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fnamew);
        return;
    }
    // opened!
    // random parameters
    for (i = 0; i < 10; i++)
        randSprm(pra + i);
    showSprm(pra[3]);
    // check size 800B?
    printSize(pra);
    fwrite(pra, sizeof pra, 1, fp);
    // close
    fclose(fp);
}

// ファイルを読み込んで配列に代入
// ファイル名を構築して, loadSprmFileDirect を実行するだけ
int loadSprmFileFlex(const char *format, int gene_num, Sprm *pra, size_t pra_size) {
    char fnamer[FILENAME_MAX];
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    return loadSprmFileDirect(fnamer, pra, pra_size);
}

// n は個体数
// マクロのPOPULATIONは使わずに可変とする
void checkSprmFileFlex(const char *format, int gene_num, int n) {
    Sprm pra[n];
    int t = loadSprmFileFlex(format, gene_num, pra, sizeof pra);
    //printDecimal(t);
    if (t < 0)
        return;
    // 読み込めたら表示
    showSprm(pra[0]);
}

// pythonでパラメータを読み取りたい
// 値を代入するための float のポインタを与える(長さ64の配列)
// ファイル名も指定する
int getTopSprmPy(const char *fnamer, float f_pointer[MASU_NUM]) {
    // 10個のパラメータを読み込むが, 使うのは1位だけ
    Sprm pra[SURVIVE_NUM];
    if (loadSprmFileDirect(fnamer, pra, sizeof pra) < 0)
        return -1;
    // 適切な位置にパラメータを配置
    for (int i = 0; i < MASU_NUM; i++)
        // 先頭のパラメータから
        f_pointer[i] = pra[0].weight[CORR_TABLE[i]];
    return 0;
}

// トップ10の平均値を取得(共有ライブラリ用)
// 世代番号でなく, ファイル名で指定する
// ファイル名の世代番号指定はpythonで行う
void getTop10AveFlexPy(const char *fnamer, float f_pointer[SPRM_LEN]) {
    FILE *fp;
    // ファイルを読み込み用で開く
    if ((fp = fopen(fnamer, "rb")) == NULL) {
        // 失敗
        printf("%s can't be opened.\n", fnamer);
        return;
    }
    // ファイルに保存してあるSprm配列を格納
    Sprm pa[SURVIVE_NUM];
    fread(pa, sizeof pa, 1, fp);
    fclose(fp);
    // Top10 の各マスの合計値を代入する配列
    float weight_sum[SPRM_LEN];
    // 0 で初期化
    for (int i = 0; i < SPRM_LEN; i++)
        weight_sum[i] = 0.0;
    for (int i = 0; i < SURVIVE_NUM; i++) {
        // 評価値の合計に足していく
        for (int j = 0; j < SPRM_LEN; j++) {
            weight_sum[j] += pa[i].weight[j];
        }
    }
    // 確認用
    // データ数だけ割って, 平均値にする
    // ここで引数を登場させるの忘れてた
    for (int i = 0; i < SPRM_LEN; i++)
        f_pointer[i] = weight_sum[i] / 10;
}

// パラメータ毎に平均値を計算する関数
void calcSprmMeans(const Sprm *family, float means[SPRM_LEN], int n) {
    // 0 で初期化 (float配列用関数)
    zerosFloat(means, SPRM_LEN);
    // 和を計算
    for (int i = 0; i < n; i++)
        for (int j = 0; j < SPRM_LEN; j++)
            means[j] += family[i].weight[j];
    // 個体数で割る
    for (int j = 0; j < SPRM_LEN; j++)
        means[j] /= n;
}

// パラメータ毎に標準偏差を計算する関数
// 世代の全てのデータを使うことを想定して, 個体数で割ることにする
void calcSprmSD(const Sprm *family, float SD[SPRM_LEN], int n) {
    float means[SPRM_LEN];
    // まずは平均値を計算
    calcSprmMeans(family, means, n);
    // 初期化 (float配列用関数)
    zerosFloat(SD, SPRM_LEN);
    // 偏差の2乗の和を計算
    for (int i = 0; i < n; i++)
        for (int j = 0; j < SPRM_LEN; j++)
            SD[j] += powf(family[i].weight[j] - means[j], 2.0);
    // 個体数で割って平方根を計算
    for (int j = 0; j < SPRM_LEN; j++)
        SD[j] = sqrtf(SD[j] / n);
}

// 世代全体の平均値を取得
// 個体数は可変にしたいので長さも引数として与える
// 無効なファイル名が渡されたときの処理も追加
// pythonでいちいち関数増やすのもだるいので, 圧縮版も兼ねてしまおう
// 引数を一つ増やして識別したほうが簡単そうだ…多分
// compressed が0なら非圧縮, 0以外なら圧縮
int getFamilyMeansPy(const char *fnamer, float f_pointer[SPRM_LEN], int n, int compressed) {
    Sprm family[n];
    int e;
    switch (compressed) {
        case 0: // 非圧縮版
            e = loadSprmFileDirect(fnamer, family, sizeof family);
            break;
        default: // 圧縮版
            e = loadSprmFileCompDirect(fnamer, family, n);
    }
    // フラグが負ならエラー
    if (e < 0) return -1;
    // 平均値計算
    calcSprmMeans(family, f_pointer, n);
    return 0;
}

// トップ10の標準偏差を取得(共有ライブラリ用)
// 世代番号でなく, ファイル名で指定する
// ファイル名を世代番号でしていするのはpythonで行なう
// 標本標準偏差?を使うためにデータ数-1で割ることにする
void getTop10SDFlexPy(const char *fnamer, float f_pointer[SPRM_LEN]) {
    FILE *fp;
    // ファイルを読み込み用で開く
    if ((fp = fopen(fnamer, "rb")) == NULL) {
        // 失敗
        printf("%s can't be opened.\n", fnamer);
        return;
    }
    // ファイルに保存してあるSprm配列を格納
    Sprm pa[SURVIVE_NUM];
    fread(pa, sizeof pa, 1, fp);
    fclose(fp);
    // Top10 の各マスの平均値を代入する配列
    float weight_mean[SPRM_LEN];
    // 0 で初期化
    zerosFloat(weight_mean, SPRM_LEN);
    zerosFloat(f_pointer, SPRM_LEN);
    // まずは合計値を計算
    // 添え字の対応に注意
    // i は個体番号, j はマス番号
    for (int i = 0; i < SURVIVE_NUM; i++)
        for (int j = 0; j < SPRM_LEN; j++)
            weight_mean[j] += pa[i].weight[j];

    // 個体数で割って平均値を計算
    for (int i = 0; i < SPRM_LEN; i++)
        weight_mean[i] /= SURVIVE_NUM;
    
    // 偏差の2乗の合計値を求める
    for (int i = 0; i < SURVIVE_NUM; i++)
        for (int j = 0; j < SPRM_LEN; j++)
            f_pointer[j] += powf(pa[i].weight[j] - weight_mean[j], 2.0f);
    
    // 個体数-1 で割り, 平方根を取って標準偏差にする
    for (int i = 0; i < SPRM_LEN; i++)
        f_pointer[i] = sqrtf(f_pointer[i] / (SURVIVE_NUM - 1));
}

// ある世代全個体の標準偏差を取得
// 個体数以下の数をnに指定することも可能
// 圧縮版か否かを判別する引数を追加
int getFamilySDPy(const char *fnamer, float f_pointer[SPRM_LEN], int n, int compressed) {
    Sprm family[n];
    int e;
    switch (compressed) {
        case 0: // 非圧縮
            e = loadSprmFileDirect(fnamer, family, sizeof family);
            break;
        default: // 圧縮
            e = loadSprmFileCompDirect(fnamer, family, n);
    }
    if (e < 0) return -1;
    showSprmOneLine(*family);
    // 標準偏差計算
    calcSprmSD(family, f_pointer, n);
    return 0;
}

// あるファイルの先頭パラメータ (エリート選択されていると仮定すると, トップパラメータ)
// がランダムAIと対戦したときの (勝ち数, 引き分け数, 負け数) を返したい
// 白黒は引数として与える, トータル処理は python に任せたい
// 個体数と試合数を引数で指定
// 代表者取得関数も使えたが, エラー判定ができないのでやめておく
// 指し手決定関数を決める識別子?も与える
// 0: 固定, 1: ルーレット
int getTopSprmGameRsltVSRandPy(const char *fnamer, int color, int loc_pop, int game_num, int decNxt_id, int result[3]) {
    printString(fnamer);
    Sprm family[loc_pop];
    if (loadSprmFileDirect(fnamer, family, sizeof family) < 0)
        return -1; // ロード失敗
    // ロード成功
    int winner;
    // 指し手決定関数
    decNxtSprm dnfunc;
    // 結果を代入する配列を初期化
    zeros(result, 3);
    // チェック
    showSprmOneLine(*family);
    if (color == 0b01) {
        printf("black\n");
    } else {
        printf("white\n");
    }
    // 指し手決定関数の設定
    // 現状は0とそれ以外で区別
    switch (decNxt_id) {
        case 0: // IDが0なら固定
            dnfunc = getBestBoardForBlackSimple;
            printf("best\n");
            break;
        default: // IDが0以外ならルーレット
            dnfunc = getBoardForBlackSimpleRoulette;
            printf("roulette\n");
    }
    for (int i = 0; i < game_num; i++) {
        // 勝者取得
        // 配列をそのまま与える -> 先頭のポインタを与える
        winner = SprmVSRandomNormal(dnfunc, family, color);
        // 勝ち
        if (winner == color) {
            result[0]++;
        }
        // 引き分け
        else if (winner == 0) {
            result[1]++;
        }
        // 負け
        else {
            result[2]++;
        }
    }
    return 0;
}

// python で使うときにまず実行する
void initPy(void) {
    // デフォは時間でシードを設定
    srand((unsigned)time(NULL));
    // これはまじで必須
    setCORR_TABLE();
    // 念のため
    initBoard();
}

// シード値設定
void setSeedPy(unsigned int seed) {
    //printf("%u\n", seed);
    //printf("%x\n", seed);
    srand(seed);
}

// 乱数チェック
void checkRandPy(void) {
    printDecimal(rand());
}

// 初期化できているか確認
void showStartPy(void) {
    showBoard(START);
}
