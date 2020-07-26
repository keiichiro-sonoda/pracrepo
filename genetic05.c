#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "genetic02.h"
#include "sort01.h"

// play against random
// return winner
int sprmVSRandomNormal(const Sprm *prp, int my_turn) {
    Board nba[NEXT_MAX];
    int kc[3];
    int pass = 0;
    int n, dif;
    // set turn
    int turn = 0b01;
    // set initial board
    Board main_board = START;
    while (1) {
        // calculate next (and normalize)
        n = nextBoardNormal2(main_board, nba, kc);
        // can't put a piece anywhere
        if (n == 0) {
            // can't do anything one another
            if (pass) {
                break;
            }
            // pass
            swapNormalizeBoard(&main_board);
            turn ^= 0b11;
            pass = 1;
            continue;
        }
        pass = 0;
        // determine a next board
        // parameter's turn
        if (turn == my_turn) {
            main_board = getBestBoardForBlackSimple(nba, n, prp);
        } // random turn
        else {
            // randomly choose a next board
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

// use Sprm[100]
// win: +2, draw: +1, lose: 0
// play against random AI 100 times for each parameter
void calcGoodnessVSRandom(Sprm *generation, int *result) {
    int i, j;
    // reset result
    zeros(result, GENE_NUM);
    // black index
    for (i = 0; i < GENE_NUM; i++) {
        for (j = 0; j < 50; j++) {
            // parameter's turn is black (50 times)
            switch(sprmVSRandomNormal(generation + i, 1)) {
                // black won
                case 1:
                    result[i] += 2;
                    break;
                // draw
                case 0:
                    result[i]++;
                // white won
                default:
                    ;
            }
            // parameter's turn is white (50 times)
            switch(sprmVSRandomNormal(generation + i, 2)) {
                // white won
                case 2:
                    result[i] += 2;
                    break;
                // draw
                case 0:
                    result[i]++;
                // black won
                default:
                    ;
            }
        }
    }
}

// choose survivors from Sprm[100]
// and show match results
// use calcGoodnessVSRandom()
void getSurvivorSprmVSRandom(Sprm *generation, Sprm *survivors) {
    int i, j;
    int result[GENE_NUM];
    int number[GENE_NUM];
    float dist;
    // number = {0, 1, 2, ..., 99}
    for (i = 0; i < GENE_NUM; i++)
        number[i] = i;
    // game! (vs random AI)
    calcGoodnessVSRandom(generation, result);
    // sort (descending order)
    quicksortDD(result, number, 0, GENE_NUM - 1);
    // show ranking
    printf("rank change\n");
    for (i = 0; i < GENE_NUM; i++) {
        // rank 11 .. 99 aren't displayed
        if (10 <= i && i < GENE_NUM - 1) continue;
        // worst!
        if (i == GENE_NUM - 1)
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
}

int main(void) {
    // initial configurations
    initBoard();
    setIndexes();
    srand((unsigned)time(NULL));

    printString("test");
    return 0;
}