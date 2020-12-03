// define some functions for selection and crossing
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort01.h"
#include "genetic02.h"

// ルーレット選択
// 選ばれた親のペアに対し, 平均化交叉と一様交叉を一回ずつ行う
// roulette selection
// perform averaging and uniform crossing once for each parents
void rouletteAveUni(const int *fitness, const int *numbers, const Sprm *current, Sprm *next) {
    int count;
    int parents[2];
    // for 文に変更してみる
    // 途中でもcountがインクリメントされるため, ループ毎+2になるはず
    // selection and crossover
    for (count = ELITE_NUM; count < POPULATION; count++) {
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
    }
}

// random selection
// only use uniform crossing
void randUni(const int *fitness, const int *numbers, const Sprm *current, Sprm *next) {
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
        // make a child (uniform crossover)
        next[count] = makeChildCrossMSprm(current[parents[0]], current[parents[1]]);
        count++;
    }
}

// roulette selection
// single point crossover
void rouletteSP(const int *fitness, const int *numbers, const Sprm *current, Sprm *next) {
    int count = ELITE_NUM;
    int parents[2];
    Sprm children[2];
    // selection and crossover
    while (count < POPULATION) {
        // choose parents by roulette selection
        // con't allow duplication
        rouletteIntMltDep(fitness, POPULATION, parents, 2);
        // make 2 children (single point crossover)
        // the mutation rate is 5%
        singlePointCrossover(current[numbers[parents[0]]], current[numbers[parents[1]]], children, 0.05f);
        next[count] = children[0];
        count++;
        // overflow
        if (count >= POPULATION) break;
        next[count] = children[1];
        count++;
    }
}

// roulette selection
// uniform crossover
// random mutation
void rouletteUni(const int *fitness, const int *numbers, const Sprm *current, Sprm *next) {
    int parents[2];
    for (int count = ELITE_NUM; count < POPULATION; count++) {
        // choose parents by roulette selection
        // con't allow duplication
        rouletteIntMltDep(fitness, POPULATION, parents, 2);
        // make a child (uniform crossover)
        next[count] = makeChildCrossMSprm(current[numbers[parents[0]]], current[numbers[parents[1]]]);
    }
}

// roulette selection
// uniform crossover
// shift mutation (no limit)
void rouletteUniSft(const int *fitness, const int *numbers, const Sprm *current, Sprm *next) {
    int parents[2];
    for (int count = ELITE_NUM; count < POPULATION; count++) {
        rouletteIntMltDep(fitness, POPULATION, parents, 2);
        // uniform crossover (no mutation)
        next[count] = uniCrossSprm(current[numbers[parents[0]]], current[numbers[parents[1]]]);
        // shift mutation (no limit)
        shiftMutNoLim(next + count);
    }
}

// roulette selection
// uniform crossover
// shift mutation (limited)
void rouletteUniSftLim(const int *fitness, const int *numbers, const Sprm *current, Sprm *next) {
    int parents[2];
    for (int count = ELITE_NUM; count < POPULATION; count++) {
        rouletteIntMltDep(fitness, POPULATION, parents, 2);
        // uniform crossover (no mutation)
        next[count] = uniCrossSprm(current[numbers[parents[0]]], current[numbers[parents[1]]]);
        // shift mutation (limited)
        shiftMutLimited(next + count);
    }
}

// roulette selection
// uniform crossover
// perform shift mutation (limited) after random mutation
void rouletteUniRdSftLim(const int *fitness, const int *numbers, const Sprm *current, Sprm *next) {
    int parents[2];
    for (int count = ELITE_NUM; count < POPULATION; count++) {
        rouletteIntMltDep(fitness, POPULATION, parents, 2);
        // uniform crossover (include radom mutation)
        next[count] = makeChildCrossMSprm(current[numbers[parents[0]]], current[numbers[parents[1]]]);
        // shift mutation (limited)
        shiftMutLimited(next + count);
    }
}

// roulette selection
// uniform crossover
// perform shift mutation (no limit) after random mutation
void rouletteUniRdSftNoLim(const int *fitness, const int *numbers, const Sprm *current, Sprm *next) {
    int parents[2];
    for (int count = ELITE_NUM; count < POPULATION; count++) {
        rouletteIntMltDep(fitness, POPULATION, parents, 2);
        // uniform crossover (include radom mutation)
        next[count] = makeChildCrossMSprm(current[numbers[parents[0]]], current[numbers[parents[1]]]);
        // shift mutation (no limit)
        shiftMutNoLim(next + count);
    }
}

// ランダム選択 (つまり適応度に依存しない)
// 一様交叉
// ランダム突然変異後, 制限なしシフト突然変異を行う
void randUniRdSftNoLim(const int *fitness, const int *numbers, const Sprm *current, Sprm *next) {
    int parents[2];
    for (int count = ELITE_NUM; count < POPULATION; count++) {
        randIntDoubleDep(parents, 0, POPULATION - 1);
        next[count] = makeChildCrossMSprm(current[parents[0]], current[parents[1]]);
        shiftMutNoLim(next + count);
    }
}

// 全て突然変異
// 選択・交叉という概念がない (エリート選択はある)
// 特殊パターンなのでマクロ MUT_RATE は使わない
void allMutation(const int *fitness, const int *numbers, const Sprm *current, Sprm *next) {
    // エリート以外全てランダム
    for (int count = ELITE_NUM; count < POPULATION; count++)
        randSprm(next + count);
}

int main(void) {
    initSprm();
    showCORR_TABLE();
    showSprmOneLine(SAMP_PRM);
    // シード固定に注意
    //srand((unsigned)time(NULL));
    // 初期設定
    const char format[] = FNF_10010001000005;
    printString(format);
    //makeFirstGeneFileFlex(format);
    //checkSprmFile(format, 20);
    //nGeneSSAFlexLoopSeed(rouletteAveUni, format, 0, 19, 81);
    sortTest();
    printString("end");
    return 0;
}