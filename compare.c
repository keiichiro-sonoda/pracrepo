#include <stdio.h>
#include <stdlib.h>
#include "genetic01.h"
#include "genetic02.h"
#include "sort01.h"

// confront Sprm and Prm1L (normalized board)
// the next board is determined by roulette each other
int SprmVSPrm1LNormal(Sprm p1, Prm1L p2, int p1_color) {
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
        // Sprm
        if (turn == p1_color) {
            printf("p1's turn\n");
            main_board = getBoardForBlackSimpleRoulette(nba, n, &p1);
        } // Prm1L
        else {
            printf("p2's turn\n");
            main_board = getBoardForBlackPrm1LRlt(nba, n, p2);
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

int main(void) {
    srand(SEED);
    printf("Hello\n");
    Sprm spr1;
    Prm1L pr1l1;
    spr1 = loadRepSprm(FNF_05006000020005, 100, 50);
    showSprmOneLine(spr1);
    pr1l1 = loadRepPrm1L("yeah", 100, 50);
    showPrm1L(pr1l1);
    return 0;
}