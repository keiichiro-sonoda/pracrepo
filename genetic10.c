#include <time.h>
#include "genetic01.h"
#include "sort01.h"

// ルーレット選択
// ブロック毎一様交叉
// ランダム突然変異
// ソート済みの個体配列を用いる場合 numbers は不要だが互換性のため
void rltUniBlRd(const int *fitness, const int *numbers, const Prm1L *current, Prm1L *next) {
    int parents[2];
    for (int count = ELITE_NUM; count < POPULATION; count ++) {
        rouletteIntMltDep(fitness, POPULATION, parents, 2);
        // ブロック毎一様交叉 (突然変異なし)
        next[count] = uniCrossBlockPrm1L(current[numbers[parents[0]]], current[numbers[parents[1]]]);
        // 値毎ランダム突然変異
        randMutPrm1L(next + count);
    }
}

// ルーレット選択
// 一点交叉 (2人っ子) 
// ランダム突然変異
void rltSPRd(const int *fitness, const int *numbers, const Prm1L *current, Prm1L *next) {
    int parents[2];
    Prm1L children[2];
    for (int count = ELITE_NUM; count < POPULATION; count++) {
        rouletteIntMltDep(fitness, POPULATION, parents, 2);
        // 一点交叉 (突然変異なし)
        singlePCross(current + numbers[parents[0]], current + numbers[parents[1]], children);
        // 値毎ランダム突然変異
        for (int i = 0; i < 2; i++)
            randMutPrm1L(children + i);
        next[count] = children[0];
        count++;
        // オーバーフロー
        if (count >= POPULATION) break;
        next[count] = children[1];
    }
}

int main(void) {
    // 初期化にシード設定も含まれる
    initPrm1LComp();
    srand((unsigned)time(NULL));
    //showBoard(START);
    const char format[] = FNF_TEST;
    printString(format);
    // for debugging
    //makeFGFilePrm1LComp(format);
    //nGenePrm1LCompLoop(rltSPRd, format, 1, 0, 101);
    crossTest();
    for (int i = 0; i < 0x7fffffff; i++) {
        if (randIntRange(0, PRM1L_LEN - 1) == PRM1L_LEN) {
            printf("yeah!\n");
            break;
        }
    }
    //sortTest();
    return 0;
}