#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

#include "genetic01.h"
#include "sort01.h"

// global variables

// ファイルの長さ
// ソート済みフラグ追加
const int PRM1L_COMP_LEN = PRM1L_LEN * POPULATION + 1;

// functions

// Prm1Lを圧縮して保存するGAを使用するときに
// 最初に行う設定関数 (ファイル長設定)
// おまけに盤面も初期化してくれる
void initPrm1LComp(void) {
    initBoard();
    printDecimal(PRM1L_COMP_LEN);
}

// black: +1, empty: 0, white: -1
// add an element for bias
void board2arraySymmetryPlus(Board src, int dst[MASU_NUM + 1]) {
    board2arraySymmetry(src, dst);
    // for bias
    // ここで倍率を設定する
    dst[MASU_NUM] = BIAS_MAG;
}

// convert weight array to Prm1L
void array2Prm1L(float src[PRM1L_LEN], Prm1L *dst) {
    int c = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j <= MASU_NUM; j++) {
            dst->weight1[i][j] = src[c];
            c++;
        }
    }
    for (int i = 0; i < 8; i++) {
        dst->weight2[i] = src[c];
        c++;
    }
}

// convert Prm1L to a weight array
void Prm1L2array(Prm1L *src, float dst[PRM1L_LEN]) {
    int c = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j <= MASU_NUM; j++) {
            dst[c] = src->weight1[i][j];
            c++;
        }
    }
    for (int i = 0; i < 8; i++) {
        dst[c] = src->weight2[i];
        c++;
    }
}

// show parameters (for Prm1L)
void showPrm1L(Prm1L pr) {
    printString("weight1:");
    for (int i = 0; i < 8; i++) {
        //printFloatArray(pr.weight1[i], MASU_NUM + 1);
        printFloatArrayPart(pr.weight1[i], MASU_NUM + 1);
    }
    printString("weight2:");
    printFloatArray(pr.weight2, 8);
}

// create random parameters (for Prm1L)
void randPrm1L(Prm1L *prp) {
    float tmp[PRM1L_LEN];
    randWeightArray(tmp, PRM1L_LEN);
    array2Prm1L(tmp, prp);
}

// calculate point (with Prm1L)
// the more advantageous to black, the higher the score
float evalWithPrm1L(Board b, Prm1L pr) {
    int inputs[MASU_NUM + 1];
    // middle points
    float pa1[8];
    // output point
    float output = 0.0f;
    // convert board to array
    board2arraySymmetryPlus(b, inputs);
    // initialization
    zerosFloat(pa1, 8);
    // first layer
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j <= MASU_NUM; j++) {
            pa1[i] += inputs[j] * pr.weight1[i][j];
        }
        // activate
        pa1[i] = ACT_FUNC(pa1[i]);
    }
    //printFloatArray(pa1, 8);
    // output layer
    for (int i = 0; i < 8; i++)
        output += pa1[i] * pr.weight2[i];
    return output;
}

// assume that the next turn is black
// n: the number of next boards
// use Prm1L
// decide next board by roulette
Board getBoardForBlackPrm1LRlt(Board *next_boards, int n, Prm1L pr) {
    float exp_points[n];
    for (int i = 0; i < n; i++) {
        // evaluate all next boards
        // and calculate the power of e (to make numbers positive)
        // sign inversion!
        exp_points[i] = expf(-evalWithPrm1L(next_boards[i], pr) * 10);
    }
    return next_boards[rouletteFloat(exp_points, n, sumFloat(exp_points, n))];
}

// return winner
// give a function pointer to determine the next board (with Prm1L)
// boards are normalized
int oneToOneNPrm1LFlex(Board (*decNxt)(Board*, int, Prm1L), Prm1L spr, Prm1L gpr) {
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
            main_board = decNxt(nba, n, spr);
        } // white (second)
        else {
            //printf("white\n");
            main_board = decNxt(nba, n, gpr);
        }
        // switch turn
        turn ^= 0b11;
    }
    // difference between black and white
    dif = kc[1] - kc[2];
    if (dif > 0) return turn;
    if (dif < 0) return turn ^ 0b11;
    // draw
    return 0;
}

// play against random
// return winner
// boards are normalized
int Prm1LVSRandomNormal(Prm1L pr, int my_color) {
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
        // Prm1L's turn
        if (turn == my_color) {
            // determine the next board by roulette
            //printf("black\n");
            main_board = getBoardForBlackPrm1LRlt(nba, n, pr);
        } // random
        else {
            //printf("white\n");
            main_board = nba[rand() % n];
        }
        // switch turn
        turn ^= 0b11;
    }
    // difference between black and white
    dif = kc[1] - kc[2];
    if (dif > 0) return turn;
    if (dif < 0) return turn ^ 0b11;
    // draw
    return 0;
}

// calculate win rate when playing against random AI
// n: number of games
float calcWinRatePrm1LVSRand(Prm1L pr, int pr_color, int n) {
    int count = 0;
    for (int i = 0; i < n; i++)
        if (Prm1LVSRandomNormal(pr, pr_color) == pr_color)
            count++;
    return (float)count / n;
}

// check win rate when playing against random AI
// n: number of games in each color??
void checkWinRatePrm1LVSRand(Prm1L pr, int n) {
    float rb, rw;
    rb = calcWinRatePrm1LVSRand(pr, 0b01, n);
    rw = calcWinRatePrm1LVSRand(pr, 0b10, n);
    printf("the number of games: %d x 2\n", n);
    printf("win rate (black, white, total): %4.1f%%, %4.1f%%, %4.1f%%\n", rb * 100, rw * 100, (rb + rw) * 50);
}

// with Prm1L[POPULATION]
// win: +2, draw: +1, lose: 0
// give a function pointer to decide the next board
void leagueMatchPrm1LFlex(Board (*decNxt)(Board*, int, Prm1L), const Prm1L *family, int *result) {
    // set all elements to zero
    zeros(result, POPULATION);
    // black index
    for (int i = 0; i < POPULATION; i++) {
        //printf("\ai = %d / %d", i, FAMILY_NUM);
        // white index
        for (int j = 0; j < POPULATION; j++) {
            if (i == j) continue;
            switch(oneToOneNPrm1LFlex(decNxt, family[i], family[j])) {
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

// write parameters to a file
// give a file name for writing
// be careful of overwriting
int dumpPrm1LDirect(const char *fname, Prm1L *pra, size_t pra_size) {
    FILE *fp;
    if ((fp = fopen(fname, "wb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fname);
        return -1;
    }
    fwrite(pra, pra_size, 1, fp);
    fclose(fp);
    return 0;
}

// パラメータを圧縮して書き込む (ファイル名直接指定)
// 上書きに注意
// フラグを渡す
int dumpPrm1LCompDirect(const char *fname, Prm1L *pra, char flag) {
    FILE *fp;
    float w_arr[PRM1L_COMP_LEN - 1];
    // フラグを含む配列を用意
    char comp_pra[PRM1L_COMP_LEN];
    // Prm1L配列を重みの一次元配列にまとめる
    for (int i = 0; i < POPULATION; i++) {
        Prm1L2array(pra + i, w_arr + i * PRM1L_LEN);
    }
    // char型に圧縮
    weight2charArray(w_arr, comp_pra, PRM1L_COMP_LEN - 1);
    // 末尾にフラグを代入
    comp_pra[PRM1L_COMP_LEN - 1] = flag;
    // 書き込み用で開く
    if  ((fp = fopen(fname, "wb")) == NULL) {
        // 失敗
        printf("%s can't be opened.\n", fname);
        return -1;
    }
    // 成功
    fwrite(comp_pra, PRM1L_COMP_LEN, 1, fp);
    fclose(fp);
    return 0;
}

// 圧縮して書き込み (ファイルフォーマットと世代番号を与える)
// フラグを渡す
int dumpPrm1LComp(const char *format, int generation, Prm1L *pra, char flag) {
    char fnamew[FILENAME_MAX];
    snprintf(fnamew, FILENAME_MAX, format, generation);
    return dumpPrm1LCompDirect(fnamew, pra, flag);
}

// make first generation file (Prm1L)
// give a file name format
// record all individuals
int makeFGFilePrm1L(const char *format) {
    char fnamew[FILENAME_MAX];
    snprintf(fnamew, FILENAME_MAX, format, 0);
    // check existence
    if (warnOverwriting(fnamew) < 0)
        return -1;
    // make new family randomly
    Prm1L pra[POPULATION];
    for (int i = 0; i < POPULATION; i++)
        randPrm1L(pra + i);
    // write
    if (dumpPrm1LDirect(fnamew, pra, sizeof pra) < 0)
        return -1;
    printf("%ld bytes were written\n", sizeof pra);
    return 0;
}

// 圧縮バージョンで最初の世代を作成
// float型を仲介せずに, ただchar型乱数配列を書き込む
int makeFGFilePrm1LComp(const char *format) {
    char fnamew[FILENAME_MAX];
    snprintf(fnamew, FILENAME_MAX, format, 0);
    // 上書き注意
    if (warnOverwriting(fnamew) < 0)
        return -1;
    FILE *fp;
    // 圧縮されたPrm1Lの配列を格納
    // 一次元配列として扱う
    char comp_pra[PRM1L_COMP_LEN];
    // -127から127のchar型乱数を代入 (フラグ部分以外)
    randWeightCharArray(comp_pra, PRM1L_COMP_LEN - 1);
    // フラグは0で初期化
    comp_pra[PRM1L_COMP_LEN - 1] = 0;
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        // 失敗
        printf("%s can't be opened.\n", fnamew);
        return -1;
    }
    // 書き込んで閉じる
    fwrite(comp_pra, PRM1L_COMP_LEN, 1, fp);
    fclose(fp);
    // 書き込まれたサイズを表示
    printf("%ld bytes were written\n", sizeof comp_pra);
    return 0;
}

// read parameters from a file (Prm1L)
// give a file name directly
int loadPrm1LDirect(const char *fname, Prm1L *pra, size_t pra_size) {
    FILE *fp;
    if ((fp = fopen(fname, "rb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fname);
        return -1;
    }
    fread(pra, pra_size, 1, fp);
    fclose(fp);
    return 0;
}

// read parameters from a file (Prm1L)
int loadPrm1L(const char *format, int gene_num, Prm1L *pra, size_t pra_size) {
    char fnamer[FILENAME_MAX];
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    return loadPrm1LDirect(fnamer, pra, pra_size);
}

// 圧縮ファイルをロード
// ファイル名直接記入バージョン
// 読み込めた場合, ソートフラグを返す
int loadPrm1LCompDirect(const char *fname, Prm1L *pra) {
    FILE *fp;
    if ((fp = fopen(fname, "rb")) == NULL) {
        // 失敗
        printf("%s can't be opened.\n", fname);
        return -1;
    }
    // 成功
    char comp_pra[PRM1L_COMP_LEN];
    fread(comp_pra, PRM1L_COMP_LEN, 1, fp);
    fclose(fp);
    // フラグ部分を除く
    float w_arr[PRM1L_COMP_LEN - 1];
    // 重み配列に変換
    char2weightArray(comp_pra, w_arr, PRM1L_COMP_LEN - 1);
    // 配列を個体数に分割してPrm1L配列に代入
    for (int i = 0; i < POPULATION; i++) {
        // ポインタをPrm1Lの長さだけずらす
        array2Prm1L(w_arr + i * PRM1L_LEN, pra + i);
    }
    // ソートフラグを返す (char型なので一応intにする)
    return (int)(comp_pra[PRM1L_COMP_LEN - 1]);
}

// 圧縮されたファイルからPrm1Lの配列を取得
int loadPrm1LComp(const char *format, int gene_num, Prm1L *pra) {
    char fnamer[FILENAME_MAX];
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    return loadPrm1LCompDirect(fnamer, pra);
}

// load a representative of Prm1L
Prm1L loadRepPrm1L(const char *format, int gene_num, int loc_pop) {
    Prm1L pra[loc_pop];
    if (loadPrm1L(format, gene_num, pra, sizeof pra) < 0) {
        // failed
        // set random parameters
        randPrm1L(pra);
    }
    return *pra;
}

// view parematers in a file (Prm1L)
void checkPrm1LFile(const char *format, int gene_num) {
    Prm1L pra[POPULATION];
    if (loadPrm1L(format, gene_num, pra, sizeof pra) < 0)
        return;
    // show only beggining and end
    showPrm1L(pra[0]);
    showPrm1L(pra[POPULATION - 1]);
}

// uniform crossover (Prm1L)
Prm1L uniCrossPrm1L(Prm1L mother, Prm1L father) {
    float m_arr[PRM1L_LEN], f_arr[PRM1L_LEN], c_arr[PRM1L_LEN];
    Prm1L child;
    // convert to array
    Prm1L2array(&mother, m_arr);
    Prm1L2array(&father, f_arr);
    // uniform cross over
    uniCrossArray(m_arr, f_arr, c_arr, PRM1L_LEN);
    // convert to Prm1L
    array2Prm1L(c_arr, &child);
    return child;
}

// uniform crossover (Prm1L)
// including random mutation
Prm1L uniCrossRMPrm1L(Prm1L mother, Prm1L father) {
    float m_arr[PRM1L_LEN], f_arr[PRM1L_LEN], c_arr[PRM1L_LEN];
    Prm1L child;
    // convert to array
    Prm1L2array(&mother, m_arr);
    Prm1L2array(&father, f_arr);
    // uniform cross over
    uniCrossArray(m_arr, f_arr, c_arr, PRM1L_LEN);
    // random mutation
    randMutArray(c_arr, PRM1L_LEN);
    // convert to Prm1L
    array2Prm1L(c_arr, &child);
    return child;
}

// roulette selection
// uniform crossover
// random mutation
void rltUniRd(const int *fitness, const int *numbers, const Prm1L *current, Prm1L *next) {
    int count = ELITE_NUM;
    int parents[2];
    while (count < POPULATION) {
        rouletteIntMltDep(fitness, POPULATION, parents, 2);
        //printf("%d, %d, %d, %d\n", parents[0], parents[1], numbers[parents[0]], numbers[parents[1]]);
        next[count] = uniCrossRMPrm1L(current[numbers[parents[0]]], current[numbers[parents[1]]]);
        count++;
    }
}

// make next generation file
// give a function pointer for selection, crossover and mutation
int nGenePrm1L(scmFuncPrm1L scm, const char *format, int gene_num, int safety) {
    char fname[FILENAME_MAX];
    // the file name to be read
    snprintf(fname, FILENAME_MAX, format, gene_num);
    printf("read file : %s\n", fname);
    // current family
    Prm1L current[POPULATION];
    // load data
    if (loadPrm1LDirect(fname, current, sizeof current) < 0)
        return -1;
    // the file name to be written
    snprintf(fname, FILENAME_MAX, format, gene_num + 1);
    printf("write file: %s\n", fname);
    // don't allow overwriting
    if (safety && warnOverwriting(fname) < 0)
        return -1;
    // next family
    Prm1L next[POPULATION];
    // the array to store points
    int fitness[POPULATION];
    // individual numbers
    int numbers[POPULATION];
    // numbers = {0, 1, 2, ...}
    indices(numbers, POPULATION);
    // evaluate fitness (with Prm1L)
    // the next board is decided by roulette
    leagueMatchPrm1LFlex(getBoardForBlackPrm1LRlt, current, fitness);
    // sort (descending order)
    randomizedQuicksortDDAll(fitness, numbers, POPULATION);
    // show the all or part of results
    printString("results:");
    printDecimalArray(fitness, POPULATION);
    //printDecimalArrayPart(fitness, POPULATION);
    // elite selection
    for (int i = 0; i < ELITE_NUM; i++)
        next[i] = current[numbers[i]];
    // selection and crossing
    scm(fitness, numbers, current, next);
    // view the part of top parameter
    printString("the top of this generation:");
    showPrm1L(next[0]);
    // 逐一勝率チェックしたい場合はこれを実行
    //checkWinRatePrm1LVSRand(next[0], 500);
    // write next family to the file
    // and return error flag
    return dumpPrm1LDirect(fname, next, sizeof next);
}

// 圧縮されたファイルから個体を読み出し, 適応度を評価する
// 適応度降順に個体を並び替え, 同じファイルに書き込む
// 適応度はルーレット選択等に用いるため, 呼び出し元で配列を渡す
int sortPrm1LCompFileByFitness(const char *fname, int *fitness) {
    // ソート前とソート後のパラメータ配列を用意する (メモリの無駄遣いかな?)
    Prm1L pra1[POPULATION], pra2[POPULATION];
    // ロードしてフラグを取得
    int flag = loadPrm1LCompDirect(fname, pra1);
    // エラーかソート済みならフラグを返す
    if (flag) return flag;
    // 個体番号を割り振る
    int numbers[POPULATION];
    indices(numbers, POPULATION);
    // リーグ戦 (指し手ルーレット)
    // 現状で適応度評価はこれだけ
    leagueMatchPrm1LFlex(getBoardForBlackPrm1LRlt, pra1, fitness);
    // 適応度を基に個体番号も同時にソート
    randomizedQuicksortDDAll(fitness, numbers, POPULATION);
    // 適応度順に並び替えてpra2に代入
    for (int i = 0; i < POPULATION; i++)
        pra2[i] = pra1[numbers[i]];
    // ソート後の配列を同じファイルに書き戻す (ソート済みフラグを立てる)
    return dumpPrm1LCompDirect(fname, pra2, 1);
}

// 次の世代のファイルを作る関数 (圧縮バージョン)
// ついでに適応度評価をした現世代のファイルもソートして書き換える (あとで使えそう)
int nGenePrm1LComp(scmFuncPrm1L scm, const char *format, int gene_num, int safety) {
    // 読み込み (ソート) 用と書き込み用ファイル名
    char fnames[FILENAME_MAX], fnamew[FILENAME_MAX];
    snprintf(fnames, FILENAME_MAX, format, gene_num);
    snprintf(fnamew, FILENAME_MAX, format, gene_num + 1);
    printf("sort file: %s\n", fnames);
    printf("new file : %s\n", fnamew);
    // 上書き拒否
    if (safety && (warnOverwriting(fnamew) < 0))
        return -1;
    int fitness[POPULATION];
    // 適応度評価とファイルのソート
    int flag = sortPrm1LCompFileByFitness(fnames, fitness);
    printDecimal(flag);
    // エラー
    if (flag < 0) return -1;
    // ソート済み
    if (flag == 1) {
        printf("%s is already sorted!\n", fnames);
        // 仮適応度
        for (int i = 0; i < POPULATION; i++)
            fitness[i] = POPULATION - i;
    }
    srand(SEED);
    // なんとなくソート済み適応度を表示
    printDecimalArray(fitness, POPULATION);
    // 今世代と次世代の個体配列を宣言
    Prm1L current[POPULATION], next[POPULATION];
    // 個体配列自体がソートされているため不要だが, 旧仕様との互換性のために定義
    int numbers[POPULATION];
    indices(numbers, POPULATION);
    // ソート済み配列を読み込む
    if (loadPrm1LCompDirect(fnames, current) < 0)
        return -1;
    // エリートはそのままコピー
    copyArray(current, next, ELITE_NUM);
    // 選択, 交叉, 突然変異
    scm(fitness, numbers, current, next);
    // トップパラメータを見る
    printString("the top of this generation:");
    showPrm1L(next[0]);
    // ソート済みフラグは立てずに書き込み
    return dumpPrm1LCompDirect(fnamew, next, 0);
}

// with Prm1L
void nGenePrm1LLoop(scmFuncPrm1L scm, const char *format, int safety, int st, int loop) {
    time_t t0, t1;
    // get start time
    time(&t0);
    Prm1L rep_pr;
    for (int i = st; i < st + loop; i++) {
        // set the generation number as the seed
        srand((unsigned)i);
        if (nGenePrm1L(scm, format, i, safety) < 0) {
            // error
            printString("error");
            return;
        }
        // たまに勝率計算
        if (i % 20 == 0) {
            kugiri(100);
            rep_pr = loadRepPrm1L(format, i, POPULATION);
            checkWinRatePrm1LVSRand(rep_pr, 500);
        }
        // get time
        time(&t1);
        printf("elapsed time: %lds\n", t1 - t0);
        kugiri(100);
    }
}

// with Prm1L
// use SEED
void nGenePrm1LLoopSeed(scmFuncPrm1L scm, const char *format, int safety, int st, int loop) {
    time_t t0, t1;
    // get start time
    time(&t0);
    unsigned int s1;
    // たまに使うパラメータを代入する変数
    Prm1L rep_pr;
    for (int i = st; i < st + loop; i++) {
        // use the number of generation as a part of seed
        s1 = i * SEED;
        // set seed
        printf("seed: %d\n", s1);
        srand((unsigned)s1);
        if (nGenePrm1L(scm, format, i, safety) < 0) {
            // error
            printString("error");
            return;
        }
        // たまに勝率計算
        if (i % 20 == 0) {
            kugiri(100);
            rep_pr = loadRepPrm1L(format, i, POPULATION);
            checkWinRatePrm1LVSRand(rep_pr, 500);
        }
        // get time
        time(&t1);
        printf("elapsed time: %lds\n", t1 - t0);
        kugiri(100);
    }
}