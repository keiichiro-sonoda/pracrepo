// use roulette to decide next action!
// also use roulette selection instead of elite selection
// ranking selection
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>

#include "genetic02.h"
#include "sort01.h"

// choose survivors[10] from generation[100]
// and show match results
// select survivors by roulette
// use the points directly
void getSurvivorSprmRltRlt(const Sprm *generation, Sprm *survivors) {
    // a function that determine the next board
    Board (*decNxt)(Board*, int, const Sprm*);
    decNxt = getBoardForBlackSimpleRoulette;
    // the array to store points
    int result[GENE_NUM];
    int number[GENE_NUM];
    int lucky[SURVIVE_NUM];
    // number = {0, 1, 2, ..., 99}
    indices(number, GENE_NUM);
    // game (the next board is decided by roulette)
    leagueMatchSprmFlex(decNxt, generation, result);
    // sort (descending order)
    // not always necessary
    // make the results easy to understand
    quicksortDD(result, number, 0, GENE_NUM - 1);
    // roulette selection!
    // don't allow duplication
    rouletteIntMltDep(result, GENE_NUM, lucky, SURVIVE_NUM);
    // view result
    printf("selected ranking:\n");
    printDecimalArray(lucky, SURVIVE_NUM);
    // save the survivors and view
    printf("the survived parameters view:\n");
    for (int i = 0; i < SURVIVE_NUM; i++) {
        survivors[i] = generation[number[lucky[i]]];
        printFloatArray(survivors[i].weight, SPRM_LEN);
    }
    checkSprmStatistics(survivors, SURVIVE_NUM);
}

// choose survivors[10] from generation[100]
// and show match results
// select survivors by roulette
// all points are subtract by the minimum point (and add 1 to all points)
void getSurvivorSprmRRSubMin(const Sprm *generation, Sprm *survivors) {
    Board (*decNxt)(Board*, int, const Sprm*);
    decNxt = getBoardForBlackSimpleRoulette;
    // the array to store points
    int result[GENE_NUM];
    int number[GENE_NUM];
    int lucky[SURVIVE_NUM];
    int min_sub1;
    // number = {0, 1, 2, ..., 99}
    indices(number, GENE_NUM);
    // game (the next board is decided by roulette)
    leagueMatchSprmFlex(decNxt, generation, result);
    // sort (descending order)
    // not always necessary
    // make the results easy to understand
    quicksortDD(result, number, 0, GENE_NUM - 1);

    printDecimalArrayPart(result, GENE_NUM);
    min_sub1 = result[GENE_NUM - 1] - 1;
    subConst(result, GENE_NUM, min_sub1);
    printDecimalArrayPart(result, GENE_NUM);

    // roulette selection!
    // don't allow duplication
    rouletteIntMltDep(result, GENE_NUM, lucky, SURVIVE_NUM);
    // view result
    printf("selected ranking:\n");
    printDecimalArray(lucky, SURVIVE_NUM);
    // save the survivors and view
    printf("the survived parameters view:\n");
    for (int i = 0; i < SURVIVE_NUM; i++) {
        survivors[i] = generation[number[lucky[i]]];
        printFloatArray(survivors[i].weight, SPRM_LEN);
    }
    checkSprmStatistics(survivors, SURVIVE_NUM);
}


// choose survivors[10] from generation[100]
// and show match results
// select survivors by ranking selection
void getSurvivorSprmRRank(const Sprm *generation, Sprm *survivors) {
    // a function that determine the next board
    Board (*decNxt)(Board*, int, const Sprm*);
    decNxt = getBoardForBlackSimpleRoulette;
    // the array to store points
    int result[GENE_NUM];
    int number[GENE_NUM];
    int lucky[SURVIVE_NUM];
    int min_sub1;
    // number = {0, 1, 2, ..., 99}
    indices(number, GENE_NUM);
    // game (the next board is decided by roulette)
    leagueMatchSprmFlex(decNxt, generation, result);
    // sort (descending order)
    // not always necessary
    // make the results easy to understand
    quicksortDD(result, number, 0, GENE_NUM - 1);

    printDecimalArrayPart(result, GENE_NUM);
    min_sub1 = result[GENE_NUM - 1] - 1;
    subConst(result, GENE_NUM, min_sub1);
    printDecimalArrayPart(result, GENE_NUM);

    // roulette selection!
    // don't allow duplication
    rouletteIntMltDep(result, GENE_NUM, lucky, SURVIVE_NUM);
    // view result
    printf("selected ranking:\n");
    printDecimalArray(lucky, SURVIVE_NUM);
    // save the survivors and view
    printf("the survived parameters view:\n");
    for (int i = 0; i < SURVIVE_NUM; i++) {
        survivors[i] = generation[number[lucky[i]]];
        printFloatArray(survivors[i].weight, SPRM_LEN);
    }
    checkSprmStatistics(survivors, SURVIVE_NUM);
}

int main(void) {
    // initialize global variables
    setIndexes();
    initBoard();
    makeSprmSample();

    int (*nGeneF)(void(), const char*, int, int);
    void (*getSvr)(const Sprm*, Sprm*);
    nGeneF = nextGenerationSprmFlex;
    getSvr = getSurvivorSprmRRSubMin;
    //nextGenerationSprmFlexLoopFlex(getSvr, nGeneF, FNF_RR_SUBMIN, 0, 0, 100);
    copyFGFlex(FNF_RRANK);
    checkSprmFile(FNF_RRANK, 0);
    return 0;
}