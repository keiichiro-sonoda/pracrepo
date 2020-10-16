// use roulette to decide next action!
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>

#include "genetic02.h"
#include "sort01.h"

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
    printFloatArray(exp_points, n);
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
        showBoard(main_board);
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
            printf("black\n");
            main_board = getBoardForBlackSimpleRoulette(nba, n, spp);
        } // white (second)
        else {
            printf("white\n");
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

int main(void) {
    // set seed
    srand((unsigned)time(NULL));
    // initialize global variables
    setIndexes();
    initBoard();
    Board nba[NEXT_MAX];
    Board yeah;
    int kc[] = {0, 0, 0};
    int nc;
    Sprm sample_prm;
    makeSprmSample(&sample_prm);
    //printFloatArray(sample_prm.weight, SPRM_LEN);
    oneToOneNormalSprmRoulette(&sample_prm, &sample_prm);
    return 0;
}