// this is a source file for debugging
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort01.h"
#include "genetic02.h"

// roulette selection
// perform averaging and uniform crossing once for each parents
void rouletteAveUni(const int *fitness, const int *numbers, const Sprm *current, Sprm *next) {
    int count;
    int parents[2];
    // elite selection
    for (count = 0; count < ELITE_NUM; count++)
        next[count] = current[numbers[count]];
    // selection and crossover
    while (count < POPULATION) {
        // choose parents by roulette selection
        // don't allow duplication
        rouletteIntMltDep(fitness, POPULATION, parents, 2);
        // make a child (average)
        next[count] = makeChildAverageSprm(current[numbers[parents[0]]], current[numbers[parents[1]]]);
        count++;
        // overflow
        if (count >= POPULATION) break;
        // make a child (uniform crossover)
        next[count] = makeChildCrossMSprm(current[numbers[parents[0]]], current[numbers[parents[1]]]);
        count++;
    }
}

int main(void) {
    srand(123U);
    //srand((unsigned)time(NULL));
    setIndexes();
    initBoard();
    char format[] = FNF_05006001000005;
    //makeFirstGeneFileFlex(format);
    checkSprmFile(format, 2);
    nGeneSSAFlexLoop(rouletteAveUni, format, 1, 0, 2);
    return 0;
}