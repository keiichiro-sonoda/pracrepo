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
        exp_points[i] = expf(evaluationSimple(next_boards[i], *prp));
    }
    printFloatArray(exp_points, n);
    s = sumFloat(exp_points, n);
    printFloat(s);
    choosed = rouletteFloat(exp_points, n, s);
    rouletteFloatTest(exp_points, n);
    return next_boards[choosed];
}

int main(void) {
    // set seed
    srand((unsigned)time(NULL));
    // initialize global variables
    setIndexes();
    initBoard();
    Board nbs[NEXT_MAX];
    Board yeah;
    int kc[] = {0, 0, 0};
    int nc;
    Sprm sample_prm;
    makeSprmSample(&sample_prm);
    printFloatArray(sample_prm.weight, SPRM_LEN);
    nc = nextBoardNormal2(SAMPLE1, nbs, kc);
    //showBoardArray(nbs, nc);
    yeah = getBoardForBlackSimpleRoulette(nbs, nc, &sample_prm);
    showBoard(yeah);
    return 0;
}