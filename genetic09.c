// define some functions for selection and crossing
#include <time.h>
#include <unistd.h>

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
    // グローバル変数がファイル名と対応しているか要注意
    geoProg(prob, POPULATION, 1., CMN_RATIO_EFF);
    // 1ループで子は2つなので, 毎回2を足す
    for (count = ELITE_NUM; count < POPULATION; count += 2) {
        // ルーレット選択 (重複なし)
        // ルーレットに使うのは等比数列
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

// 等比数列ランキング選択, BLX-α 交叉, ランダム突然変異, ソート済限定, 圧縮推奨
void rankGeoProgBLXaRdCS(const int *fitness, const Sprm *current, Sprm *next) {
    int count, parents[2];
    Sprm child;
    double prob[POPULATION];
    geoProg(prob, POPULATION, 1., CMN_RATIO_EFF);
    for (count = ELITE_NUM; count < POPULATION; count++) {
        // 選択
        rouletteDoubleMltDep(prob, POPULATION, parents, 2);
        // BLX-α 交叉 (圧縮対応の値に補正)
        child = blendCrossSprmComp(current + parents[0], current + parents[1]);
        // ランダム突然変異 (圧縮対応乱数)
        randMutSprmCC(&child);
        next[count] = child;
    }
}

// 関数型を返す関数
// 識別子から選択・交叉・突然変異関数 (ソート済限定) を決定する
scmSprmSorted detScmFuncSprmS(int sel_id, int crs_id, int mut_id) {
    // デフォルト
    scmSprmSorted res_func;
    // 全て合わせて switch で個別処理
    switch (sel_id * 10000 + crs_id * 100 + mut_id) {
        case 20500:
            puts("等比数列ランキング選択, 2人っ子一様交叉, ランダム突然変異");
            //CMN_RATIO_EFF = CMN_RATIO; // 公比はそのまま
            printf("公比: %f\n", CMN_RATIO_EFF);
            res_func = rankGeoProgUni2CRdS;
            break;
        case 30500:
            puts("等比数列ランキング選択 (対数表記), 2人っ子一様交叉, ランダム突然変異");
            //CMN_RATIO_EFF = exp(CMN_RATIO_LN); // 指数を計算し公比とする
            //printf("公比: exp(%+6.3f) = %f\n", CMN_RATIO_LN, CMN_RATIO_EFF);
            res_func = rankGeoProgUni2CRdS;
            break;
        case 30600:
            puts("等比数列ランキング選択 (対数表記), BLX-α 交叉, ランダム突然変異, 圧縮推奨");
            printf("BLX-α 交叉の α : %4.2f\n", ALPHA_BLX);
            res_func = rankGeoProgBLXaRdCS;
            break;
        default:
            miteigiExit(NULL);
    }
    return res_func;
}

// いくつかの公比を試す
// 公比は自然対数で与える
// stop は含まない予定だが, double型 の誤差でどうなるかわからない
int trySomeCommonRatio(double start, double stop, double step, int gene_max) {
    double loc_cr_ln;
    char format[FILENAME_MAX];
    int n, flag, gene_now;
    scmSprmSorted scm;
    time_t t0, t1;
    // 最初の時間を取得
    time(&t0);
    // ループ回数を計算
    n = (stop - start) / step;
    if (n > 0) {
        printf("試す公比の数: %d\n", n);
    } else {
        printf("s\atart, stop, step の値が不適切です. \n");
        return -1;
    }
    // 関数を決定, 存在しなければエラー
    if ((scm = detScmFuncSprmS(3, CROSSOVER_ID, 0)) == NULL) {
        return -1;
    }
    for (int i = 0; i < n; i++) {
        // 公比計算
        loc_cr_ln = start + step * i;
        // 選択方法は固定する (当然)
        // BLX-α 交叉を試したくなってしまった
        // 公比を格納するグローバル変数もここで決定
        if (makeSprmFileFormatAuto(format, FILENAME_MAX, EF_FUNC_ID, COMPRESS, POPULATION, ELITE_NUM, 3, CROSSOVER_ID, 0, MUT_RATE, SEED, loc_cr_ln, ZAKO_FIRST, ALPHA_BLX) < 0) {
            puts("フォーマット作成失敗");
            return -1;
        }
        printf("公比: exp(%+6.3f) = %f\n", loc_cr_ln, CMN_RATIO_EFF);
        printf("フォーマット: %s\n", format);
        // 現世代は 0 から (例外あり)
        gene_now = 0;
        // 初期世代作成, 必要ならばディレクトリも作成
        // 第二引数 -1 は, 初期世代作成スルーモード
        // 正常にスルーしたら -2 が返る
        if ((flag = makeFGFileSprmCompMkdir(format, -1)) == -2) {
            // 進んでいる世代数を獲得
            // 最大世代以前なら, そこから再開
            if ((gene_now = getGeneNumComp(format, POPULATION)) < gene_max) {
                printf("%d 世代目から再開します.\n", gene_now);
            }
            // 最大世代を過ぎていたら, 次の公比へ
            else {
                continue;
            }
        }
        // 普通に失敗
        else if (flag < 0) {
            puts("初期世代作成失敗");
            return -1;
        }
        // 各公比, 指定した世代までループ (安全装置なし)
        if (nGeneSprmCompLoop(scm, format, 0, gene_now, gene_max) < 0) {
            return -1;
        }
        time(&t1);
        printf("公比お試し関数開始から: ");
        printElapsedTime(t1 - t0);
        kugiri(100);
    }
    return 0;
}

int main(void) {
    // 初期設定
    initSprm();
    // シード固定に注意
    //srand((unsigned)time(NULL));
    // フォーマット自動作成 (シードまで)
    char format[FILENAME_MAX];
    if (makeSprmFileFormatAuto(format, FILENAME_MAX, EF_FUNC_ID, COMPRESS, POPULATION, ELITE_NUM, SELECTION_ID, CROSSOVER_ID, MUTATION_ID, MUT_RATE, SEED, ZAKO_FIRST, CMN_RATIO_LN, ALPHA_BLX) < 0) {
        puts("フォーマット作成失敗");
        return -1;
    }
    searchSeedSprm(EF_FUNC_ID, COMPRESS, POPULATION, ELITE_NUM, CROSSOVER_ID, MUTATION_ID, MUT_RATE, -0.005);
    // マクロのIDで関数決定
    scmSprmSorted scm = NULL;
    //scm = detScmFuncSprmS(SELECTION_ID, CROSSOVER_ID, MUTATION_ID);
    // warning 回避, ポインタの一致を確認
    printHex64(scm);
    //printHex64(rankGeoProgBLXaRdCS); printHex64(rankGeoProgUni2CRdS);
    printString(format);
    //printf("条件とファイルフォーマットは合っていますか?"); kakuninExit();
    //makeFGFileSprmComp(format);
    // ディレクトリがなければ作成する
    //makeFGFileSprmCompMkdir(format, 0);
    //checkSprmFileComp(format, 0);
    // ループ
    //nGeneSprmCompLoop(scm, format, 1, 0, 101);
    // 修正用
    //sortOnlySprmComp(scm, format, 0);
    //checkSprmFileComp(format, 0);
    //sortTest();
    //trySomeCommonRatio(-2.0, 2.2, 0.2, 101);
    //crossTestSprm();
    return 0;
}