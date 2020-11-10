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
        if (count >= POPULATION) break;
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
    if (loadSprmFile(format, gene_num, current, sizeof current) < 0) {
        return -1;
    }
    // view the file name
    printf("write file: %s\n", fnamew);
    // don't allow overwriting
    if (safety && warnOverwriting(fnamew) < 0) {
        return -1;
    }
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
    printDecimalArrayPart(fitness, POPULATION);
    // selection and crossing
    selAndCross(fitness, numbers, current, next);
    // view some individuals
    showFamilyPart(next);
    // view statistics
    checkSprmStatistics(next, POPULATION);

    // write next family to the file
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        // failed
        printf("\a%s cannot be opened\n", fnamew);
        return -1;
    }
    // opened!
    fwrite(next, sizeof next, 1, fp);
    fclose(fp);
    return 0;
}

int main(void) {
    srand(123U);
    //srand((unsigned)time(NULL));
    setIndexes();
    initBoard();

    makeFirstGeneFileFlex(FNF_TEST);
    checkSprmFile(FNF_TEST, 0);
    //nGeneSSALoopFlex(nGeneSprmSaveAll, FNF_05006000000005, 0, 0, 100);
    //sortTest();
    return 0;
}