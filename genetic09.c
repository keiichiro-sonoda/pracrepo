// define some functions for selection and crossing
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

// ルーレット選択, 平均化交叉と一様交叉一回ずつ
// 全体の突然変異率を整えるために, 一様交叉と同じ割合で平均化交叉後にも突然変異を加える
// ソート済み配列前提. さらに平均化交叉後の突然変異乱数は圧縮に対応させる
void rltAveUniEqS(const int *fitness, const Sprm *current, Sprm *next) {
    int parents[2];
    for (int count = ELITE_NUM; count < POPULATION; count++) {
        rouletteIntMltDep(fitness, POPULATION, parents, 2);
        next[count] = makeChildAverageSprm(current[parents[0]], current[parents[1]]);
        randMutSprmCC(next + count);
        if (count++ >= POPULATION) break;
        next[count] = makeChildCrossMSprm(current[parents[0]], current[parents[1]]);
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

// ルーレット選択, 一点交叉, ランダム突然変異, ソート済み個体限定
void rltSPRdS(const int *fitness, const Sprm *current, Sprm *next) {
    int parents[2];
    Sprm children[2];
    for (int count = ELITE_NUM; count < POPULATION; count++) {
        rouletteIntMltDep(fitness, POPULATION, parents, 2);
        singlePointCrossover(current[parents[0]], current[parents[1]], children, MUT_RATE);
        // インクリメントを式に組み込むことを試す
        next[count++] = children[0];
        if (count >= POPULATION) break;
        // こっちでインクリメントすると, for文でも行うので過剰になる
        next[count] = children[1];
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

// 等比数列ランキング選択, 一様交叉, ランダム突然変異, ソート済み限定, 圧縮推奨
// 適応度は使わないが互換性のため
// 研究対象
void rankGeoProgUni2CRdS(const int *fitness, const Sprm *current, Sprm *next) {
    int i, count, parents[2];
    Sprm children[2];
    double prob[POPULATION];
    // 等比数列を作成 (初項は0以外ならなんでもいい?)
    geoProg(prob, POPULATION, 1., CMN_RATIO);
    // 1ループで子は2つなので, 毎回2を足す
    for (count = ELITE_NUM; count < POPULATION; count += 2) {
        // ルーレット選択 (重複なし)
        rouletteDoubleMltDep(prob, POPULATION, parents, 2);
        // 一様交叉で2つの子を作成
        uniCrossSprm2C(current + parents[0], current + parents[1], children);
        // 子の数くり返し (0 と 1)
        // ここでオーバーフローも検知
        // 外の for 文と二重検知で効率は悪いかも
        for (i = 0; (i < 2) && (count + i < POPULATION); i++) {
            // ランダム突然変異 (圧縮対応乱数)
            randMutSprmCC(children + i);
            // 次の世代に追加
            next[count + i] = children[i];
        }
    }
}

int main(void) {
    // 初期設定
    initSprm();
    // シード固定に注意
    //srand((unsigned)time(NULL));
    char format[FILENAME_MAX];
    // このマクロの第一引数を変える
    //formatPlusSeed(FNF_TEST, format, FILENAME_MAX);
    formatPlusSeed(FNF_RDC05000_0290_0500001, format, FILENAME_MAX);
    printString(format);
    printf("条件とファイルフォーマットは合っていますか?");
    kakuninExit();
    //makeFGFileSprmComp(format);
    //checkSprmFileComp(format, 0);
    //nGeneSprmCompLoop(rltAveUniEqS, format, 1, 0, 201);
    //nGeneSprmCompLoop(rltSPRdS, format, 1, 2, 201);
    //nGeneSprmCompLoop(rltUniRdS, format, 1, 0, 3);
    // 研究対象
    //nGeneSprmCompLoop(rankGeoProgUni2CRdS, format, 1, 0, 1);
    // 修正用
    sortOnlySprmComp(rankGeoProgUni2CRdS, format, 0);
    //sortTest();
    return 0;
}