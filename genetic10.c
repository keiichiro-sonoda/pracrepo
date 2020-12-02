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

int main(void) {
    // 初期化にシード設定も含まれる
    initPrm1LComp();
    //srand((unsigned)time(NULL));
    //showBoard(START);
    const char format[] = FNF_TEST;
    printString(format);
    // for debugging
    //makeFGFilePrm1LComp(format);
    //nGenePrm1LCompLoop(rltUniBlRd, format, 1, 100, 201);
    Prm1L pra[POPULATION];
    Prm1L children[2];
    loadPrm1LComp(format, 0, pra);
    showPrm1L(pra[0]);
    showPrm1L(pra[1]);
    singlePCross(pra, pra + 1, children);
    showPrm1L(children[0]);
    showPrm1L(children[1]);
    return 0;
}