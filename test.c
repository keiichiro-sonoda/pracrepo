// this is a source file for debugging
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort01.h"
#include "genetic02.h"

// select randomly except for elite selection
// perform averaging and uniform crossing once for each parents
void randAveUni(const int *fitness, const int *numbers, const Sprm *current, Sprm *next) {
    int count;
    int parents[2];
    // elite selection
    for (count = 0; count < ELITE_NUM; count++)
        next[count] = current[numbers[count]];
    // selection and crossover
    while (count < POPULATION) {
        // choose parents randomly
        // don't allow duplication
        randIntDoubleDep(parents, 0, POPULATION - 1);
        // make a child (average)
        next[count] = makeChildAverageSprm(current[parents[0]], current[parents[1]]);
        count++;
        if (count >= POPULATION) break;
        // make a child (uniform crossover)
        next[count] = makeChildCrossMSprm(current[parents[0]], current[parents[1]]);
        count++;
    }
}

// make next generation file
// write all individuals to the file
// give a function pointer for selection and crossing
int nGeneSSAFlex(void (*selAndCross)(const int*, const int*, const Sprm*, Sprm*), const char *format, int gene_num, int safety) {
    printf("debugging\n");
    char fnamew[FILENAME_MAX];
    FILE *fp;
    // the file name to be written
    snprintf(fnamew, FILENAME_MAX, format, gene_num + 1);
    // current family
    Sprm current[POPULATION];
    // load data
    if (loadSprmFile(format, gene_num, current, sizeof current) < 0)
        return -1;
    // view the file name
    printf("write file: %s\n", fnamew);
    // don't allow overwriting
    if (safety && warnOverwriting(fnamew) < 0)
        return -1;
    // next family
    Sprm next[POPULATION];
    // the array to store points
    int fitness[POPULATION];
    // individual numbers
    int numbers[POPULATION];
    // numbers = {0, 1, 2, ...}
    indices(numbers, POPULATION);
    // evaluate fitness (the next board is decided by roulette)
    leagueMatchSprmFlex(getBoardForBlackSimpleRoulette, current, fitness);
    // sort (descending order)
    randomizedQuicksortDDAll(fitness, numbers, POPULATION);
    // show the part of fitness
    printString("results:");
    printDecimalArrayPart(fitness, POPULATION);
    // selection and crossing
    selAndCross(fitness, numbers, current, next);
    // view some individuals
    printString("next family:");
    showFamilyPart(next);
    // view statistics
    checkSprmStatistics(next, POPULATION);
    // write next family to the file
    // and return error flag
    return dumpSprmFileDirect(fnamew, next, sizeof next);
}

// give a function pointer for selection and crossover
void nGeneSSAFlexLoop(void (*selAndCross)(const int*, const int*, const Sprm*, Sprm*), const char *format, int safety, int st, int loop) {
    time_t t0, t1;
    // get start time
    time(&t0);
    for (int i = st; i < st + loop; i++) {
        // set the generation number as the seed
        srand((unsigned)i);
        if (nGeneSSAFlex(selAndCross, format, i, safety) < 0) {
            // error
            printString("error");
            return;
        }
        // get time
        time(&t1);
        printf("elapsed time: %lds\n", t1 - t0);
    }
}

int main(void) {
    srand(123U);
    //srand((unsigned)time(NULL));
    setIndexes();
    initBoard();

    //makeFirstGeneFileFlex(FNF_TEST);
    //checkSprmFile(FNF_TEST, 0);
    //nGeneSSALoopFlex(nGeneSprmSaveAll, FNF_05006000000005, 0, 0, 100);
    //sortTest();
    nGeneSSAFlexLoop(randAveUni, FNF_TEST, 1, 0, 2);
    return 0;
}