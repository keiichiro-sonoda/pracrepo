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

// assume that the next turn is black
// n: the number of next boards
// use simple parameter
// decide next board by roulette
Board getBoardForBlackSimpleRoulette(Board *next_boards, int n, const Sprm *prp) {
    float exp_points[n];
    float s;
    int choosed;
    for (int i = 0; i < n; i++) {
        // evaluate all next boards
        // and calculate the power of e (to make numbers positive)
        // sign inversion!
        exp_points[i] = expf(-evaluationSimple(next_boards[i], *prp) * 10);
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

// use Sprm[100]
// win: +2, draw: +1, lose: 0
void leagueMatchSprmRoulette(const Sprm *generation, int *result) {
    // set all elements to zero
    zeros(result, GENE_NUM);
    // black index
    for (int i = 0; i < GENE_NUM; i++) {
        //printf("\ai = %d / %d", i, FAMILY_NUM);
        // white index
        for (int j = 0; j < GENE_NUM; j++) {
            if (i == j) continue;
            // the next board is decided by roulette
            switch(oneToOneNormalSprmRoulette(generation + i, generation + j)) {
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

// choose survivors[10] from generation[100]
// and show match results
void getSurvivorSprmRoulette(const Sprm *generation, Sprm *survivors) {
    int i, j;
    // the array to store points
    int result[GENE_NUM];
    int number[GENE_NUM];
    // number = {0, 1, 2, ..., 99}
    for (i = 0; i < GENE_NUM; i++)
        number[i] = i;
    // game (the next board is decided by roulette)
    leagueMatchSprmRoulette(generation, result);
    // sort (descending order)
    quicksortDD(result, number, 0, GENE_NUM - 1);
    // show ranking
    printf("rank change\n");
    for (i = 0; i < GENE_NUM; i++) {
        // rank 11-99 aren't displayed
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
    // this value may not make sense
    printf("distance: %6.4f\n", distSprm(survivors[0], generation[0]));
    // check the top parameter
    printf("the top parameter view:\n");
    printFloatArray(survivors[0].weight, SPRM_LEN);
}

// make next generation file
int nextGenerationSprmRoulette(int gene_num, int safety) {
    int i, j, count;
    char fnamer[FILENAME_MAX], fnamew[FILENAME_MAX];
    // previous survivors
    Sprm parents[SURVIVE_NUM];
    FILE *fp;
    // the file name to be read
    snprintf(fnamer, FILENAME_MAX, FNAME_FORMAT, gene_num);
    // the file name to be written
    snprintf(fnamew, FILENAME_MAX, FNAME_FORMAT, gene_num + 1);
    // view the file names
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
    // allow overwriting
    if (!safety);
    // check the file to be written (can read?)
    else if ((fp = fopen(fnamew, "rb")) != NULL) {
        printf("\a%s exists. Do you overwrite it? (y\\n): ", fnamew);
        fclose(fp);
        // don't overwrite
        if (getchar() != 121) {
            while (getchar() != 10);
            printf("terminated\n");
            return -1;
        }
        if (getchar() != 10) {
            while (getchar() != 10);
            printf("terminated\n");
            return -1;
        }
    }
    // store a generation
    Sprm generation[GENE_NUM];

    // 10 parents copy (elite selection)
    for (count = 0; count < SURVIVE_NUM; count++)
        generation[count] = parents[count];

    // crossing! (use elite genes)
    // 45 combinations, 2 children per couple
    for (i = 0; i < SURVIVE_NUM - 1; i++) {
        for (j = i + 1; j < SURVIVE_NUM; j++) {
            // average
            generation[count] = makeChildAverageSprm(parents[i], parents[j]);
            count++;
            // cross (the mutation rate is 5%)
            generation[count] = makeChildCrossMSprm(parents[i], parents[j]);
            count++;
        }
    }
    // calcurate survivors
    Sprm survivors[SURVIVE_NUM];
    // battle! (the next board is decided by roulette)
    getSurvivorSprmRoulette(generation, survivors);

    // write current survivors to the file
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
void nextGenerationSprmRouletteLoop(int st, int loop) {
    time_t t0, t1;
    // get start time
    time(&t0);
    for (int i = st; i < st + loop; i++) {
        // set the generation number as the seed
        srand((unsigned)i);
        if (nextGenerationSprmRoulette(i, 0) == -1)
            return;
        // get time
        time(&t1);
        printf("elapsed time: %lds\n", t1 - t0);
    }
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
    // initialize global variables
    setIndexes();
    initBoard();
    makeSprmSample();
    //nextGenerationSprmRouletteLoop(0, 100);
    checkSprmFile(FNAME_FORMAT, 0);
    test1();
    printf("yeah\n");
    return 0;
}