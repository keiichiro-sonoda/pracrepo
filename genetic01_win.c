#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

#include "genetic01_win.h"
#include "sort01_win.h"

// global variables
Prm1L USE_PRM1L;
float USE_VECTOR[MASU_NUM + 1];

// functions

void sigmoidFloatP(float *xp) {
    (*xp) = sigmoidFloat(*xp);
}

// black: +1, empty: 0, white: -1
// add an element for bias
void board2arraySymmetryPlus(Board src, int dst[MASU_NUM + 1]) {
    board2arraySymmetry(src, dst);
    // for bias
    dst[MASU_NUM] = 1;
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

// シグモイドを忘れたPrm1Lを等価と予想されるベクトルに変換
void Prm1L2vector(Prm1L src, float dst[MASU_NUM + 1]) {
    // 0 で初期化
    zeros(dst, MASU_NUM + 1);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j <= MASU_NUM; j++)
            dst[j] += src.weight1[i][j] * src.weight2[i];
}

// Prm1L を使った盤面評価
// シグモイドを使うのを忘れていた!
// これではただの線形変換と変わらない
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
    }
    // ここにシグモイドやステップを挟むべきだった
    // output layer
    for (int i = 0; i < 8; i++)
        output += pa1[i] * pr.weight2[i];
    return output;
}

// ベクトルで評価
// evalWithPrm1L と結果が一致するかどうかを確認したい
float evalWithVector(Board b, float vector[MASU_NUM + 1]) {
    int inputs[MASU_NUM + 1];
    float output = 0.0f;
    board2arraySymmetryPlus(b, inputs);
    // 単純に内積を取る
    for (int i = 0; i <= MASU_NUM; i++)
        output += inputs[i] * vector[i];
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

// python で使うパラメータを設定する
// 読み込みたいファイル名を指定, その中の先頭要素を用いる
// 人数 n はpythonから渡し, 変更可能とする
int setUsePrm1LPy(const char *fname, int n) {
    Prm1L pra[n];
    // 読み込みとエラー処理
    if (loadPrm1LDirect(fname, pra, sizeof pra) < 0) {
        return -1;
    } 
    // 先頭要素を代入
    USE_PRM1L = pra[0];
    // 確認
    showPrm1L(USE_PRM1L);
    // ベクトルに変換
    Prm1L2vector(USE_PRM1L, USE_VECTOR);
    // 確認
    printFloatArray(USE_VECTOR, MASU_NUM + 1);
    return 0;
}

// pythonから与えられた盤面に対し, Prm1Lを使って手を選択する
// 今のところルーレット選択を考えている
// 引数にターンを追加
int getActPrm1LPy(int b_info[MASU_NUM], int turn) {
    printString("debugging");
    Board b;
    int n;
    // 正規化された盤面と指し手の対応表
    BoardAct baa[NEXT_MAX];
    // 配列を盤面に変換
    array2board(b_info, &b);
    showBoard(b);
    // 正味の盤面の数を求める
    n = canPutNet(b, turn, baa);
    showBoardActArray(baa, n);
    printDecimal(n);
    // 盤面の数だけ評価値配列を作る
    float exp_points[n];
    for (int i = 0; i < n; i++)
        exp_points[i] = expf(i);
    // 評価値比較
    printFloat(evalWithPrm1L(b, USE_PRM1L));
    printFloat(evalWithVector(b, USE_VECTOR));
    return 0;
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

// warning before overwriting
// importet from genetic02.c
int warnOverwriting(const char *fname) {
    FILE *fp;
    // open to read
    if ((fp = fopen(fname, "rb")) == NULL) {
        // not exist
        return 0;
    }
    // exist
    fclose(fp);
    printf("\a\"%s\" exists. Do you overwrite it? (y\\n): ", fname);
    char c = getchar();
    if (c != 121) {
        if (c != 10)
            while (getchar() != 10);
        printf("terminated\n");
        return -1;
    }
    if (getchar() != 10) {
        while (getchar() != 10);
        printf("terminated\n");
        return -1;
    }
    // allowed
    return 0;
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

// view parematers in a file (Prm1L)
void checkPrm1LFile(const char *format, int gene_num) {
    Prm1L pra[POPULATION];
    if (loadPrm1L(format, gene_num, pra, sizeof pra) < 0)
        return;
    // show only beggining and end
    showPrm1L(pra[0]);
    showPrm1L(pra[POPULATION - 1]);
}
