#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

#include "genetic01.h"
#include "sort01.h"

// global variables

// functions

void sigmoidFloatP(float *xp) {
    (*xp) = sigmoidFloat(*xp);
}

int board2array(Board src, int *dst) {
    int i;
    for (i = 0; i < 64; i++) {
        dst[i] = getKoma(src, i * 2);
    }
    return 0;
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

int checkParam(Param pr) {
    int i, j;
    // first layer
    for (i = 0; i < 32; i++) {
        printf("1-%02d: ", i);
        for (j = 0; j < 64; j++) {
            printf("%4.2f ", pr.weight1[i][j]);
        }
        printf("\n%4.2f\n", pr.bias1[i]);
    } // second layer
    for (i = 0; i < 16; i++) {
        printf("2-%02d: ", i);
        for (j = 0; j < 32; j++) {
            printf("%4.2f ", pr.weight2[i][j]);
        }
        printf("\n%4.2f\n", pr.bias2[i]);
    } // third layer
    for (i = 0; i < 8; i++) {
        printf("3-%02d: ", i);
        for (j = 0; j < 16; j++) {
            printf("%4.2f ", pr.weight3[i][j]);
        }
        printf("\n%4.2f\n", pr.bias3[i]);
    } // fourth layer
    for (i = 0; i < 4; i++) {
        printf("4-%02d: ", i);
        for (j = 0; j < 8; j++) {
            printf("%4.2f ", pr.weight4[i][j]);
        }
        printf("\n%4.2f\n", pr.bias4[i]);
    } // fifth layer
    for (i = 0; i < 2; i++) {
        printf("5-%02d: ", i);
        for (j = 0; j < 4; j++) {
            printf("%4.2f ", pr.weight5[i][j]);
        }
        printf("\n%4.2f\n", pr.bias5[i]);
    } // sixth layer
    printf("6: ");
    for (j = 0; j < 2; j++) {
        printf("%4.2f ", pr.weight6[j]);
    }
    putchar('\n');
    return 0;
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

// randomly determine parameter
int paramRand(Param *prp) {
    int i, j;
    // first layer
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 64; j++) {
            (*prp).weight1[i][j] = randWeight();
        }
        (*prp).bias1[i] = randWeight();
    } // second layer
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 32; j++) {
            (*prp).weight2[i][j] = randWeight();
        }
        (*prp).bias2[i] = randWeight();
    } // third layer
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 16; j++) {
            (*prp).weight3[i][j] = randWeight();
        }
        (*prp).bias3[i] = randWeight();
    } // fourth layer
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            (*prp).weight4[i][j] = randWeight();
        }
        (*prp).bias4[i] = randWeight();
    } // fifth layer
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            (*prp).weight5[i][j] = randWeight();
        }
        (*prp).bias5[i] = randWeight();
    } // sixth layer
    for (j = 0; j < 2; j++) {
        (*prp).weight6[j] = randWeight();
    }
    return 0;
}

// create random parameters (for Prm1L)
void randPrm1L(Prm1L *prp) {
    float tmp[PRM1L_LEN];
    randWeightArray(tmp, PRM1L_LEN);
    array2Prm1L(tmp, prp);
}

// generation initialization
int familyRand(Family *fmp) {
    int i;
    for (i = 0; i < POPULATION; i++) {
        paramRand((*fmp).prms + i);
    }
    return 0;
}

// calculate point
// the more advantageous to black, the higher the score
float evaluation(Board b, Param pr) {
    int i, j;
    int pa0[64];
    float pa1[32], pa2[16], pa3[8], pa4[4], pa5[2];
    // output point
    float pa6 = 0;
    // input array
    board2array(b, pa0);
    // first layer
    for (i = 0; i < 32; i++) {
        // initialization
        pa1[i] = pr.bias1[i];
        for (j = 0; j < 64; j++)
            pa1[i] += pa0[j] * pr.weight1[i][j];
        sigmoidFloatP(pa1 + i);
    } // second layer
    for (i = 0; i < 16; i++) {
        pa2[i] = pr.bias2[i];
        for (j = 0; j < 32; j++)
            pa2[i] += pa1[j] * pr.weight2[i][j];
        sigmoidFloatP(pa2 + i);
    } // third layer
    for (i = 0; i < 8; i++) {
        pa3[i] = pr.bias3[i];
        for (j = 0; j < 16; j++)
            pa3[i] += pa2[j] * pr.weight3[i][j];
        sigmoidFloatP(pa3 + i);
    } // fourth layer
    for (i = 0; i < 4; i++) {
        pa4[i] = pr.bias4[i];
        for (j = 0; j < 8; j++)
            pa4[i] += pa3[j] * pr.weight4[i][j];
        sigmoidFloatP(pa4 + i);
    } // fifth layer
    for (i = 0; i < 2; i++) {
        pa5[i] = pr.bias5[i];
        for (j = 0; j < 4; j++)
            pa5[i] += pa4[j] * pr.weight5[i][j];
        sigmoidFloatP(pa5 + i);
    } // sixth layer
    pa6 = 0;
    for (j = 0; j < 2; j++)
        pa6 += pa5[j] * pr.weight6[j];
    return pa6;
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

Board getBestBoard(Board *next_boards, int next_count, int color, Param prm) {
    float mx_point = -FLT_MAX;
    float t_point;
    int i;
    Board best_board;
    // check all boards
    for (i = 0; i < next_count; i++) {
        // evaluate a board
        t_point = evaluation(next_boards[i], prm);
        // if the color is white, reverse the sign
        if (color == 0b10) t_point = -t_point;
        if (mx_point < t_point) {
            // update
            mx_point = t_point;
            best_board = next_boards[i];
        }
    }
    return best_board;
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

// return winnter
// draw: 0, black: 1, white: 2
int oneToOne(Param sente, Param gote) {
    int winner;
    int cpa[NEXT_MAX];
    Board nba[NEXT_MAX];
    int kc[3];
    int pass = 0;
    int nc;
    // black
    int turn = 0b01;
    Board main_board = START;
    while (1) {
        nc = canPutPP(main_board, turn, cpa, nba, kc);
        //showBoard(main_board);
        // cannot put a piece anywhere
        if (nc == 0) {
            // cannot do anything each other
            if (pass) {
                break;
            }
            // pass
            turn ^= 0b11;
            pass = 1;
            continue;
        }
        pass = 0;
        // black
        if (turn == 0b01) {
            main_board = getBestBoard(nba, nc, 0b01, sente);
        } // white
        else {
            main_board = getBestBoard(nba, nc, 0b10, gote);
        }
        // next turn;
        turn ^= 0b11;
    }
    //showBoard(main_board);
    // the winner is white
    if (kc[1] < kc[2]) winner = 2;
    // black
    else if (kc[1] > kc[2]) winner = 1;
    // draw
    else winner = 0;
    return winner;
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

int leagueMatch(Family fml) {
    int i, j, k;
    // index, color, (d, w, l)
    int result[POPULATION][2][3];
    // all zero
    for (i = 0; i < POPULATION; i++)
        for (j = 0; j < 2; j++)
            for (k = 0; k < 3; k++)
                result[i][j][k] = 0;
    // black index
    for (i = 0; i < POPULATION; i++) {
    //for (i = 0; i < 10; i++) {
        //printf("\ai = %d / %d\n", i, POPULATION);
        // white index
        for (j = 0; j < POPULATION; j++) {
        //for (j = 0; j < 10; j++) {
            switch(oneToOne(fml.prms[i], fml.prms[j])) {
                // black won
                case 1:
                    result[i][0][1]++;
                    result[j][1][2]++;
                    break;
                // white won
                case 2:
                    result[i][0][2]++;
                    result[j][1][1]++;
                    break;
                // draw
                default:
                    result[i][0][0]++;
                    result[j][1][0]++;
            }
        }
    }
    // show result
    for (i = 0; i < POPULATION; i++) {
        printf("no.%02d: ", i);
        for (j = 0; j < 2; j++) {
            if (j) printf("w: ");
            else printf("b: ");
            for (k = 0; k < 3; k++) {
                printf("%2d ", result[i][j][k]);
            }
        }
        putchar('\n');
    }
    return 0;
}

// win: +2, draw: +1, lose: 0
int leagueMatchSimple(Family fml, int *result) {
    int i, j;
    // all zero
    zeros(result, POPULATION);
    // black index
    for (i = 0; i < POPULATION; i++) {
        //printf("\ai = %d / %d", i, POPULATION);
        // white index
        for (j = 0; j < POPULATION; j++) {
            if (i == j) continue;
            switch(oneToOne(fml.prms[i], fml.prms[j])) {
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
    // show results
    /*
    for (i = 0; i < POPULATION; i++) {
        printf("no.%02d: %3d\n", i, result[i]);
    }
    */
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
int dumpPrm1LCompDirect(const char *fname, Prm1L *pra) {
    FILE *fp;
    int total_length = PRM1L_LEN * POPULATION;
    float w_arr[total_length];
    char comp_pra[total_length];
    // Prm1L配列を重みの一次元配列にまとめる
    for (int i = 0; i < POPULATION; i++) {
        Prm1L2array(pra + i, w_arr + i * PRM1L_LEN);
    }
    // char型に圧縮
    weight2charArray(w_arr, comp_pra, total_length);
    // 書き込み用で開く
    if  ((fp = fopen(fname, "wb")) == NULL) {
        // 失敗
        printf("%s can't be opened.\n", fname);
        return -1;
    }
    // 成功
    fwrite(comp_pra, sizeof comp_pra, 1, fp);
    fclose(fp);
    return 0;
}

// 圧縮して書き込み (ファイルフォーマットと世代番号を与える)
int dumpPrm1LComp(const char *format, int generation, Prm1L *pra) {
    char fnamew[FILENAME_MAX];
    snprintf(fnamew, FILENAME_MAX, format, generation);
    return dumpPrm1LCompDirect(fnamew, pra);
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
    // 個体全体の重みの総数
    int total_length = PRM1L_LEN * POPULATION;
    // 圧縮されたPrm1Lの配列を格納
    // 一次元配列として扱う
    char comp_pra[total_length];
    // -127から127のchar型乱数を代入
    randWeightCharArray(comp_pra, total_length);
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        // 失敗
        printf("%s can't be opened.\n", fnamew);
        return -1;
    }
    // 書き込んで閉じる
    fwrite(comp_pra, sizeof comp_pra, 1, fp);
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

// ファイル名直接記入バージョン
int loadPrm1LCompDirect(const char *fname, Prm1L *pra) {
    FILE *fp;
    if ((fp = fopen(fname, "rb")) == NULL) {
        // 失敗
        printf("%s can't be opened.\n", fname);
        return -1;
    }
    // 成功
    int total_length = PRM1L_LEN * POPULATION;
    char comp_pra[total_length];
    fread(comp_pra, sizeof comp_pra, 1, fp);
    fclose(fp);
    float w_arr[total_length];
    // 重み配列に変換
    char2weightArray(comp_pra, w_arr, total_length);
    // 配列を個体数に分割してPrm1L配列に代入
    for (int i = 0; i < POPULATION; i++) {
        // ポインタをPrm1Lの長さだけずらす
        array2Prm1L(w_arr + i * PRM1L_LEN, pra + i);
    }
    return 0;
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

int readResultFile(int *result, int r_size, char *fnamel) {
    FILE *fp;
    // read file
    if ((fp = fopen(fnamel, "rb")) == NULL)
        printf("\a%s cannot be opened\n", fnamel);
    else {
        fread(result, r_size, 1, fp);
        fclose(fp);
    }
    return 0;
}

// choose survivors
// shallow copy?
int getSurvivor(Family *fmlp, Param *survivors) {
    int i, j;
    int result[POPULATION];
    int number[POPULATION];
    float dist;
    // number = {0, 1, 2, ...}
    indices(number, POPULATION);
    // game!
    leagueMatchSimple(*fmlp, result);
    // use data (for test)
    //readResultFile(result, sizeof result, "temporary_storage000.bin");
    // sort
    quicksortDD(result, number, 0, POPULATION - 1);
    // show ranking
    printf("rank change\n");
    for (i = 0; i < ELITE_NUM; i++) {
        j = number[i];
        printf("%3d", j + 1);
        if (j < 10) printf("(p)");
        else printf("(c)");
        printf(" -> ");
        printf("%2d: %3dpt\n", i + 1, result[i]);
        survivors[i] = (*fmlp).prms[j];
    }
    printf("worst: %3dpt\n", result[99]);
    //putchar('\n');
    // calculate the distance between the previous top and the current top
    dist = paramDistance((*fmlp).prms[0], survivors[0]);
    printf("distance: %6.4f\n", dist);
    return 0;
}

float faverage(float f1, float f2) {
    return (f1 + f2) / 2;
}

Param makeChildAverage(Param mother, Param father) {
    Param child;
    int i, j;
    // first layer
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 64; j++) {
            child.weight1[i][j] = faverage(mother.weight1[i][j], father.weight1[i][j]);
        }
        child.bias1[i] = faverage(mother.bias1[i], father.bias1[i]);
    } // second layer
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 32; j++) {
            child.weight2[i][j] = faverage(mother.weight2[i][j], father.weight2[i][j]);
        }
        child.bias2[i] = faverage(mother.bias2[i], father.bias2[i]);
    } // third layer
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 16; j++) {
            child.weight3[i][j] = faverage(mother.weight3[i][j], father.weight3[i][j]);
        }
        child.bias3[i] = faverage(mother.bias3[i], father.bias3[i]);
    } // fourth layer
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            child.weight4[i][j] = faverage(mother.weight4[i][j], father.weight4[i][j]);
        }
        child.bias4[i] = faverage(mother.bias4[i], father.bias4[i]);
    } // fifth layer
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            child.weight5[i][j] = faverage(mother.weight5[i][j], father.weight5[i][j]);
        }
        child.bias5[i] = faverage(mother.bias5[i], father.bias5[i]);
    } // sixth layer
    for (j = 0; j < 2; j++) {
        child.weight6[j] = faverage(mother.weight6[j], father.weight6[j]);
    }
    return child;
}

float fcrossM(float f1, float f2) {
    // random number (0 ~ 199)
    int r = rand() % 200;
    // 0 ~ 98 (49.5%)
    if (r < 99) return f1;
    // 99 ~ 197 (49.5%)
    if (r < 198) return f2;
    // mutation!!
    // 198 or 199 (1%)
    return randWeight();
}

Param makeChildCrossM(Param mother, Param father) {
    Param child;
    int i, j;
    // first layer
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 64; j++) {
            child.weight1[i][j] = fcrossM(mother.weight1[i][j], father.weight1[i][j]);
        }
        child.bias1[i] = fcrossM(mother.bias1[i], father.bias1[i]);
    } // second layer
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 32; j++) {
            child.weight2[i][j] = fcrossM(mother.weight2[i][j], father.weight2[i][j]);
        }
        child.bias2[i] = fcrossM(mother.bias2[i], father.bias2[i]);
    } // third layer
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 16; j++) {
            child.weight3[i][j] = fcrossM(mother.weight3[i][j], father.weight3[i][j]);
        }
        child.bias3[i] = fcrossM(mother.bias3[i], father.bias3[i]);
    } // fourth layer
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            child.weight4[i][j] = fcrossM(mother.weight4[i][j], father.weight4[i][j]);
        }
        child.bias4[i] = fcrossM(mother.bias4[i], father.bias4[i]);
    } // fifth layer
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            child.weight5[i][j] = fcrossM(mother.weight5[i][j], father.weight5[i][j]);
        }
        child.bias5[i] = fcrossM(mother.bias5[i], father.bias5[i]);
    } // sixth layer
    for (j = 0; j < 2; j++) {
        child.weight6[j] = fcrossM(mother.weight6[j], father.weight6[j]);
    }
    return child;
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

// square distance
float sqDist(float f1, float f2) {
    return (float)pow(f1 - f2, 2.0);
}

float paramDistance(Param p1, Param p2) {
    float dist = 0.0;
    int i, j;
    // first layer
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 64; j++) {
            dist += sqDist(p1.weight1[i][j], p2.weight1[i][j]);
        }
        dist += sqDist(p1.bias1[i], p2.bias1[i]);
    } // second layer
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 32; j++) {
            dist += sqDist(p1.weight2[i][j], p2.weight2[i][j]);
        }
        dist += sqDist(p1.bias2[i], p2.bias2[i]);
    } // third layer
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 16; j++) {
            dist += sqDist(p1.weight3[i][j], p2.weight3[i][j]);
        }
        dist += sqDist(p1.bias3[i], p2.bias3[i]);
    } // fourth layer
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            dist += sqDist(p1.weight4[i][j], p2.weight4[i][j]);
        }
        dist += sqDist(p1.bias4[i], p2.bias4[i]);
    } // fifth layer
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            dist += sqDist(p1.weight5[i][j], p2.weight5[i][j]);
        }
        dist += sqDist(p1.bias5[i], p2.bias5[i]);
    } // sixth layer
    for (j = 0; j < 2; j++) {
        dist += sqDist(p1.weight6[j], p2.weight6[j]);
    }
    dist /= PARAM_NUM;
    return (float)sqrt(dist);
}

int nextGeneration(int gene_num) {
    int i, j, count;
    char format[] = "generation%03d.bin";
    char fnamer[FILENAME_MAX], fnamew[FILENAME_MAX];
    Param parents[ELITE_NUM];
    FILE *fp;
    Family pgene, cgene;
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    snprintf(fnamew, FILENAME_MAX, format, gene_num + 1);
    printf("read file: %s\n", fnamer);
    printf("write file: %s\n", fnamew);
    // read parent generation
    if ((fp = fopen(fnamer, "rb")) == NULL) {
        printf("\a%s can't be opened\n", fnamer);
        return -1;
    } else {
        fread(&pgene, sizeof pgene, 1, fp);
        fclose(fp);
    }
    // check write file
    if ((fp = fopen(fnamew, "rb")) != NULL) {
        printf("\a%s exists. Can I overwrite it? (y): ", fnamew);
        if (getchar() == 'y') {
            fclose(fp);
        } else {
            fclose(fp);
            return -1;
        }
    }
    // ranking
    getSurvivor(&pgene, parents);
    // take over the top 10 directly
    for (count = 0; count < ELITE_NUM; count++)
        cgene.prms[count] = parents[count];
    // make children
    // 45 combinations, 2 children per couple
    for (i = 0; i < ELITE_NUM - 1; i++) {
        for (j = i + 1; j < ELITE_NUM; j++) {
            cgene.prms[count] = makeChildAverage(parents[i], parents[j]);
            count++;
            cgene.prms[count] = makeChildCrossM(parents[i], parents[j]);
            count++;
        }
    }
    //printIntDec(count);

    // write child generation
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        printf("\a%s cannot be opened\n", fnamew);
        return -1;
    } else {
        fwrite(&cgene, sizeof cgene, 1, fp);
        fclose(fp);
    }
    return 0;
}

// roulette selection
// uniform crossover
// random mutation
void rltUniRd(const int *fitness, const int *numbers, const Prm1L *current, Prm1L *next) {
    int count = ELITE_NUM;
    int parents[2];
    while (count < POPULATION) {
        rouletteIntMltDep(fitness, POPULATION, parents, 2);
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
// 適応度はルーレット選択等に用いるため保存
int sortPrm1LCompFileByFitness(const char *fname, int *fitness) {
    Prm1L pra[POPULATION];
    return 0;
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