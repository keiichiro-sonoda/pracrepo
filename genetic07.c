// use roulette to decide next action!
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>

#include "genetic02.h"
#include "sort01.h"

// the file name format
#define FNAME_FORMAT "prm/sprm_roulette%03d.bin"

#define macroTest(src) ((14 - (src) % 16) * 8 + (src) / 16 * 2)

// assume that the next turn is black
// n: the number of next boards
// use simple parameter
// use roulette selection
Board getBoardForBlackSimpleRoulette(Board *next_boards, int n, const Sprm *prp) {
    float exp_points[n];
    float s;
    int choosed;
    for (int i = 0; i < n; i++) {
        // evaluate all next boards
        // and calculate the power of e (to make numbers positive)
        exp_points[i] = expf(evaluationSimple(next_boards[i], *prp) * 10);
    }
    //printFloatArray(exp_points, n);
    s = sumFloat(exp_points, n);
    choosed = rouletteFloat(exp_points, n, s);
    return next_boards[choosed];
}

// return winner
// the next board is decided by roulette
int oneToOneNormalSprmRoulette(const Sprm *spp, const Sprm *gpp) {
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
            main_board = getBoardForBlackSimpleRoulette(nba, n, spp);
        } // white (second)
        else {
            //printf("white\n");
            main_board = getBoardForBlackSimpleRoulette(nba, n, gpp);
        }
        // switch the turn
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

// copy the first generation
void copyFGRoulette(void) {
    FILE *fp;
    // file name for reading (source)
    char fnamer[] = "prm/simple_prm000.bin";
    if ((fp = fopen(fnamer, "rb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fnamer);
        return;
    }
    // opened!
    Sprm pa[SURVIVE_NUM];
    // temporary substitution
    fread(pa, sizeof pa, 1, fp);
    fclose(fp);
    // check the top parameter
    showSprm(pa[0]);
    // check
    printFloatArray(pa[0].weight, SPRM_LEN);
    // file name for writing (destination)
    char fnamew[FILENAME_MAX];
    snprintf(fnamew, FILENAME_MAX, FNAME_FORMAT, 0);
    // open a file to write (or make a file)
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fnamew);
        return;
    }
    // opened!
    fwrite(pa, sizeof pa, 1, fp);
    // close
    fclose(fp);
}

int main(void) {
    // set seed
    srand((unsigned)time(NULL));
    // initialize global variables
    setIndexes();
    initBoard();
    makeSprmSample();
    int j;
    int R[3];
    zeros(R, 3);
    //showSprm(SAMP_PRM);
    //printFloatArray(SAMP_PRM.weight, SPRM_LEN);
    for (int i = 0; i < 1000; i++) {
        j = oneToOneNormalSprmRoulette(&SAMP_PRM, &SAMP_PRM);
        R[j]++;
    }
    printDecimalArray(R, 3);
    return 0;
}