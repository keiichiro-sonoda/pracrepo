// pythonのctypesから呼び出す共有ライブラリを作る
// ubuntu で python を実行することを考慮すると様々な問題が発生
#include <time.h>
#include <float.h>

// この中に genetic02.h と othello.h が包含されている
#include "genetic02_share.h"
#include "sort01.h"

// global variables

// functions

// Sprm のファイルフォーマットを自動生成する関数 (Python用)
// ただし研究対象ということで等比数列ランキング選択に限る
// 書き込み対象文字列, サイズ, 個体数, エリート数, 選択方法, シード値, 公比 (対数かどうかは sel_id 依存)
// いっそ世代番号まで決めてもらおう
int makeSprmFileFormatRankGeoProgPy(char *dst, int dst_size, int loc_pop, int loc_eln, int sel_id, int loc_seed, double cmn_ratio, int gene_num) {
    char tmp_str[FILENAME_MAX];
    // 固定: 対ランダム (指し手最大), 圧縮, 一様交叉, ランダム突然変異1%
    if (makeSprmFileFormatAuto(tmp_str, FILENAME_MAX, 0b10, 1, loc_pop, loc_eln, sel_id, 5, 0, .01, loc_seed, cmn_ratio) < 0) {
        return -1;
    }
    int l;
    // 世代番号を指定
    if ((l = snprintf(dst, dst_size, tmp_str, gene_num)) >= dst_size) {
        printf("\aオーバーフロー\n");
        l = -1;
    }
    printString(dst);
    return l;
}

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
    // ロードか退出
    loadFileDirectExit(fnamer, pra, pra_size);
    return 0;
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
    loadFileDirectExit(fnamer, pra, sizeof pra);
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

// 世代全体の平均値を取得
// 個体数は可変にしたいので長さも引数として与える
// 無効なファイル名が渡されたときの処理も追加
// pythonでいちいち関数増やすのもだるいので, 圧縮版も兼ねてしまおう
// 引数を一つ増やして識別したほうが簡単そうだ…多分
// compressed が0なら非圧縮, 0以外なら圧縮
int getFamilyMeansPy(const char *fnamer, float f_pointer[SPRM_LEN], int n, int compressed) {
    Sprm family[n];
    loadSprmFileDirectFlexExit(fnamer, family, n, compressed);
    double d_means[SPRM_LEN];
    // 平均値計算関数は genetic02 で再定義した (計算は double 型)
    calcSprmMeans(family, n, d_means);
    // float 配列にコピー
    copyArray(d_means, f_pointer, SPRM_LEN);
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
    loadSprmFileDirectFlexExit(fnamer, family, n, compressed);
    // 一時的に double 型で計算
    double d_means[SPRM_LEN], d_vars[SPRM_LEN];
    // まずは平均値計算
    calcSprmMeans(family, n, d_means);
    // 分散を計算
    calcSprmVariances(family, n, d_means, d_vars);
    // 標準偏差にして渡す (型もついでに float に変わる)
    sqrtArray(d_vars, f_pointer, SPRM_LEN);
    // Python への反映チェック
    //randUcharArray(f_pointer, SPRM_LEN);
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
    // ロード失敗なら抜ける
    loadFileDirectExit(fnamer, family, sizeof family);
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

// 適応度ファイルを Python から読み取る
// fitness には loc_pop 以上の領域を確保すること
int getFitnessPy(const char *fname, int *fitness, int loc_pop) {
    // マクロで自動読み取り, エラーなら -1 を返す
    loadFitnessShortDirectExit(fname, fitness, loc_pop);
    return 0;
}
