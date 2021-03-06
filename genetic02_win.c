// pythonのctypesから呼び出す共有ライブラリを作る
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <math.h>

#include "genetic02_win.h"
#include "sort01_win.h"

// global variables
Board START;
// correspondence table
// 対応表??
// シンプルパラメータに対応するように添え字を設定する
int CORR_TABLE[MASU_NUM];

// functions defined in othello.c
// 全てヘッダファイルから輸入

// functions
// パラメータをマスに対応するように表示
void showSprm(Sprm pr) {
    int i;
    float p;
    for (i = 0; i < MASU_NUM; i++) {
        p = pr.weight[CORR_TABLE[i]];
        printf("%5.2f ", p);
        if (i % 8 == 7) {
            putchar('\n');
        }
    }
}

void showFloatArray(float *fa, int n) {
    int i;
    putchar('{');
    for (i = 0; i < n; i++) {
        printf("%5.2f", fa[i]);
        if (i < n - 1) {
            printf(", ");
        }
    }
    printf("}\n");
}

int getMin(int a, int b) {
    return (a < b ? a : b);
}

int getMinArray(const int *A, int n) {
    int i;
    int min = 0x7fffffff;
    for (i = 0; i < n; i++) {
        min = getMin(A[i], min);
    }
    return min;
}

int mirrorLRAd(int src) {
    return (src / 16) * 16 + 14 - src % 16;
}

// normalize an address
int normalAd(int ad) {
    int i, eq_ads[8];
    eq_ads[0] = ad;
    eq_ads[4] = mirrorLRAd(ad);
    for (i = 0; i < 3; i++) {
        eq_ads[i + 1] = rotL90DegAd(eq_ads[i]);
        eq_ads[i + 5] = mirrorLRAd(eq_ads[i + 1]);
    }
    return getMinArray(eq_ads, 8);
}

// ad: normalized address
int ad2index(int ad) {
    if (ad < 16)
        return ad / 2;
    if (ad < 32)
        return ad / 2 - 5;
    if (ad < 48)
        return ad / 2 - 11;
    if (ad < 64)
        return ad / 2 - 18;
    return -1;
}

// -0.5 ~ 0.5
void randSprm(Sprm *prp) {
    int i;
    for (i = 0; i < SPRM_LEN; i++) {
        prp->weight[i] = (float)rand() / RAND_MAX - 0.5;
    }
}

// calculate average
Sprm makeChildAverageSprm(Sprm mother, Sprm father) {
    Sprm child;
    int i;
    for (i = 0; i < SPRM_LEN; i++) {
        child.weight[i] = (mother.weight[i] + father.weight[i]) / 2;
    }
    return child;
}

// give mutant rate
float fcrossMFlex(float a, float b, float rate) {
    // 0.0 ~ 1.0
    float r = (float)rand() / RAND_MAX;
    // mutant!
    if (r <= rate) {
        //printf("m!\n");
        return (float)rand() / RAND_MAX - 0.5;
    }
    r = (float)rand() / RAND_MAX;
    // 50 : 50
    if (r < 0.5) return a;
    return b;
}

// cross parameters?
Sprm makeChildCrossMSprm(Sprm mother, Sprm father) {
    Sprm child;
    int i;
    for (i = 0; i < SPRM_LEN; i++) {
        // rate 5%
        child.weight[i] = fcrossMFlex(mother.weight[i], father.weight[i], 0.05);
    }
    return child;
}

// グローバル変数を設定
// convert from an address to the weight index?
void setCORR_TABLE(void) {
    int i, ad;
    for (i = 0; i < MASU_NUM; i++) {
        ad = i * 2;
        CORR_TABLE[i] = ad2index(normalAd(ad));
    }
}

// caluculate point
float evaluationSimple(Board b, Sprm pr) {
    float pt = 0;
    int i;
    int ba[MASU_NUM];
    // Board -> int array
    board2arraySymmetry(b, ba);
    //showDecimalArray(ba, MASU_NUM);
    for (i = 0; i < MASU_NUM; i++) {
        // calculate inner product
        pt += ba[i] * pr.weight[CORR_TABLE[i]];
    }
    return pt;
}

// assume that the next turn is black
// n: the number of next boards
// use simple parameter
Board getBestBoardForBlackSimple(Board *next_boards, int n, const Sprm *prp) {
    float mx_point = -FLT_MAX;
    float t_point;
    int i;
    Board best_board;
    for (i = 0; i < n; i++) {
        // sign inversion!!
        t_point = -evaluationSimple(next_boards[i], *prp);
        if (mx_point < t_point) {
            // update
            mx_point = t_point;
            best_board = next_boards[i];
        }
    }
    //printf("%5.2f\n", mx_point);
    return best_board;
}

// return winner
int oneToOneNormalSprm(const Sprm *spp, const Sprm *gpp) {
    Board nba[NEXT_MAX];
    int kc[3];
    int pass = 0;
    int n, dif;
    // set turn
    int turn = 0b01;
    // set initial board
    Board main_board = START;
    while (1) {
        // calculate next
        n = nextBoardNormal2(main_board, nba, kc);
        //showBoard(main_board);
        // can't put a piece anywhere
        if (n == 0) {
            // can't do anything one another
            if (pass) {
                //printf("end\n");
                break;
            }
            // pass
            //printf("pass\n");
            swapNormalizeBoard(&main_board);
            turn ^= 0b11;
            pass = 1;
            continue;
        }
        pass = 0;
        // determine a next board
        // black (first)
        if (turn == 0b01) {
            //printf("black\n");
            main_board = getBestBoardForBlackSimple(nba, n, spp);
        } // white (second)
        else {
            //printf("white\n");
            main_board = getBestBoardForBlackSimple(nba, n, gpp);
        }
        // switch turn
        turn ^= 0b11;
    }
    // difference between black and white
    dif = kc[1] - kc[2];
    //printDecimal(dif);
    if (dif > 0) return turn;
    if (dif < 0) return turn ^ 0b11;
    // draw
    return 0;
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

// ファイル名をフォーマットと世代番号から構築せず, そのまま指定する
int loadSprmFileDirect(const char *fname, Sprm *pra, size_t pra_size) {
    FILE *fp;
    if ((fp = fopen(fname, "rb")) == NULL) {
        printf("%s can't be opened.\n", fname);
        return -1;
    }
    fread(pra, pra_size, 1, fp);
    fclose(fp);
    return 0;
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
    // 0 で初期化
    zeros(means, SPRM_LEN);
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
    // 初期化
    zeros(SD, SPRM_LEN);
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
int getFamilyMeansPy(const char *fnamer, float f_pointer[SPRM_LEN], int n) {
    Sprm family[n];
    // ファイル読み込み
    if (loadSprmFileDirect(fnamer, family, sizeof family) < 0)
        // 失敗
        return -1;
    // 成功
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
int getFamilySDPy(const char *fnamer, float f_pointer[SPRM_LEN], int n) {
    Sprm family[n];
    if (loadSprmFileDirect(fnamer, family, sizeof family) < 0)
        return -1;
    calcSprmSD(family, f_pointer, n);
    return 0;
}

// use Sprm[100]
// win: +2, draw: +1, lose: 0
void leagueMatchSimpleSprm(Sprm *generation, int *result) {
    int i, j;
    // all zero
    zeros(result, POPULATION);
    // black index
    for (i = 0; i < POPULATION; i++) {
        //printf("\ai = %d / %d", i, FAMILY_NUM);
        // white index
        for (j = 0; j < POPULATION; j++) {
            if (i == j) continue;
            switch(oneToOneNormalSprm(generation + i, generation + j)) {
                // black won
                case 1:
                    result[i] += 2;
                    break;
                // white won
                case 2:
                    result[j] += 2;
                    break;
                // draw
                default:
                    result[i]++;
                    result[j]++;
            }
        }
    }
}

// calculate distance
float distSprm(Sprm p1, Sprm p2) {
    int i;
    float d = 0.0;
    for (i = 0; i < SPRM_LEN; i++) {
        // add square distance
        d += (float)pow(p1.weight[i] - p2.weight[i], 2.0);
    }
    // divided by number of parameters
    d /= SPRM_LEN;
    // return the square root
    return (float)sqrt(d);
}

// choose survivors from Sprm[100]
// and show match results
void getSurvivorSprm(Sprm *generation, Sprm *survivors) {
    int i, j;
    int result[POPULATION];
    int number[POPULATION];
    float dist;
    // number = {0, 1, 2, ..., 99}
    for (i = 0; i < POPULATION; i++)
        number[i] = i;
    // game!
    leagueMatchSimpleSprm(generation, result);
    // sort (descending order)
    quicksortDD(result, number, 0, POPULATION - 1);
    // show ranking
    printf("rank change\n");
    for (i = 0; i < POPULATION; i++) {
        // rank 11 .. 99 aren't displayed
        if (10 <= i && i < POPULATION - 1) continue;
        // worst!
        if (i == POPULATION - 1)
            printf("        ...\n");
        // winner's index (or worst index)
        j = number[i];
        printf("%3d", j + 1);
        if (j < 10) printf("(p)");
        else printf("(c)");
        printf(" -> ");
        printf("%3d: %3dpt\n", i + 1, result[i]);
        // record winners
        if (i < 10)
            survivors[i] = generation[j];
    }
    // calculate the distance between the previous top and the current top
    dist = distSprm(survivors[0], generation[0]);
    printf("distance: %6.4f\n", dist);
    printf("top parameters view:\n");
    showSprm(survivors[0]);
}

// make next generation file
int nextGenerationSprm(int gene_num) {
    int i, j, count;
    char format[] = "prm/simple_prm%03d.bin";
    char fnamer[FILENAME_MAX], fnamew[FILENAME_MAX];
    // previous survivors
    Sprm parents[SURVIVE_NUM];
    FILE *fp;
    // read file name
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    // write file name
    snprintf(fnamew, FILENAME_MAX, format, gene_num + 1);
    // view
    printf("read file : %s\n", fnamer);
    printf("write file: %s\n", fnamew);
    // read parents
    if ((fp = fopen(fnamer, "rb")) == NULL) {
        printf("\a%s can't be opened\n", fnamer);
        return -1;
    }
    // opened!
    fread(parents, sizeof parents, 1, fp);
    fclose(fp);
    // check write file (can read?)
    if ((fp = fopen(fnamew, "rb")) != NULL) {
        printf("\a%s exists. Can I overwrite it? (y): ", fnamew);
        fclose(fp);
        // don't overwrite
        if (getchar() != 'y') {
            printf("terminated\n");
            return -1;
        }
    }
    // make children!
    Sprm generation[POPULATION];
    // 10 parents copy
    for (count = 0; count < SURVIVE_NUM; count++) {
        generation[count] = parents[count];
    }
    // make children
    // 45 combinations, 2 children per couple
    for (i = 0; i < SURVIVE_NUM - 1; i++) {
        for (j = i + 1; j < SURVIVE_NUM; j++) {
            generation[count] = makeChildAverageSprm(parents[i], parents[j]);
            count++;
            generation[count] = makeChildCrossMSprm(parents[i], parents[j]);
            count++;
        }
    }
    // calcurate survivors
    Sprm survivors[SURVIVE_NUM];
    // battle!
    getSurvivorSprm(generation, survivors);

    // write current survivors
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        printf("\a%s cannot be opened\n", fnamew);
        return -1;
    }
    // opened!
    fwrite(survivors, sizeof survivors, 1, fp);
    fclose(fp);

    return 0;
}

// loop several times
void nextGenerationSprmLoop(int st, int loop) {
    time_t t0, t1;
    // get start time
    time(&t0);
    int i;
    for (i = st; i < st + loop; i++) {
        nextGenerationSprm(i);
        // get time
        time(&t1);
        printf("elapsed time: %I64ds\n", t1 - t0);
    }
}

// python で使うときにまず実行する
void initPy(void) {
    // これは不要かも
    srand((unsigned)time(NULL));
    // これはまじで必須
    setCORR_TABLE();
    // 念のため
    initBoard();
}

// 初期化できているか確認
void showStartPy(void) {
    showBoard(START);
}

// pythonから渡される文字列を表示してみたい
void printStringPy(const char *s) {
    printString(s);
}