// 共有ライブラリ用ソースからもincludeすることにした
#include <time.h>
#include <float.h>
#include <math.h>

#include "genetic02.h"
#include "sort01.h"

// global variables
// declarations are required

int CORR_TABLE[MASU_NUM];

// 圧縮後は重みひとつあたり1バイトで計算
// フラグ分も足す
const int SPRM_FILE_SIZE_COMP = SPRM_LEN * POPULATION + 1;

Sprm SAMP_PRM;

// 指し手決定関数 (マクロのIDによって決まる)
decNxtSprm DET_FUNC;

// functions

// グローバル変数等初期化関数
// シード設定も行う
// パラメータも表示
void initSprm(void) {
    srand(SEED);
    setCORR_TABLE();
    initBoard();
    makeSprmSample();
    printf("population          : %4d\n", POPULATION);
    printf("the number of elites: %4d\n", ELITE_NUM);
    printf("mutation rate       : %4.2f\n", MUT_RATE);
    printf("parameter file size : %4d\n", SPRM_FILE_SIZE_COMP);
    printf("seed                : %4d\n", SEED);
    printf("指し手              : ");
    switch (DET_FUNC_ID) {
        case 0: // 固定
            DET_FUNC = getBestBoardForBlackSimple;
            printf("固定\n");
            break;
        default: // ルーレット
            DET_FUNC = getBoardForBlackSimpleRoulette;
            printf("ルーレット\n");
    }
}

// print a simple parameter
void showSprm(Sprm pr) {
    for (int i = 0; i < MASU_NUM; i++) {
        printf("%5.2f ", pr.weight[CORR_TABLE[i]]);
        if (i % 8 == 7)
            putchar(10);
    }
}

// show a part of parameter array
void showFamilyPart(Sprm *pra) {
    // population is 5 or less
    if (POPULATION <= 5) {
        // show all
        for (int i = 0; i < POPULATION; i++)
            showSprmOneLine(pra[i]);
        return;
    }
    // population is greater than 5
    for (int i = 0; i < 3; i++)
        showSprmOneLine(pra[i]);
    printf("                                      ...\n");
    showSprmOneLine(pra[POPULATION - 1]);
}

// get the smallest value in an array
int getMinArray(const int *A, int n) {
    int min = 0x7fffffff;
    for (int i = 0; i < n; i++)
        min = getMin(A[i], min);
    return min;
}

// normalize an address
int normalAd(int ad) {
    int i, eq_ads[8];
    eq_ads[0] = ad;
    eq_ads[4] = mirrorLRAd(ad);
    for (i = 0; i < 3; i++) {
        eq_ads[i + 1] = rotL90DegAd(eq_ads[i]);
        eq_ads[i + 5] = mirrorLRAd(eq_ads[i + 1]);
    }
    return getMinArray(eq_ads, 8);
}

// ad: normalized address
int ad2index(int ad) {
    if (ad < 16)
        return ad / 2;
    if (ad < 32)
        return ad / 2 - 5;
    if (ad < 48)
        return ad / 2 - 11;
    if (ad < 64)
        return ad / 2 - 18;
    return -1;
}

// set a parameter to a floating point random number from -0.5 to 0.5
void randSprm(Sprm *prp) {
    for (int i = 0; i < SPRM_LEN; i++) {
        prp->weight[i] = (float)rand() / RAND_MAX - 0.5;
    }
}

// calculate average
Sprm makeChildAverageSprm(Sprm mother, Sprm father) {
    Sprm child;
    int i;
    for (i = 0; i < SPRM_LEN; i++) {
        child.weight[i] = (mother.weight[i] + father.weight[i]) / 2;
    }
    return child;
}

// give mutant rate
float fcrossMFlex(float a, float b, float rate) {
    // 0.0 ~ 1.0
    float r = randFloat();
    // mutated!
    if (r <= rate) {
        return randFloat() - 0.5;
    }
    r = randFloat();
    // 50 : 50
    if (r < 0.5) return a;
    return b;
}

// uniform crossover
// include random mutation
Sprm makeChildCrossMSprm(Sprm mother, Sprm father) {
    Sprm child;
    for (int i = 0; i < SPRM_LEN; i++) {
        child.weight[i] = fcrossMFlex(mother.weight[i], father.weight[i], MUT_RATE);
    }
    return child;
}

// uniform crossover
// no mutation
Sprm uniCrossSprm(Sprm mother, Sprm father) {
    Sprm child;
    uniCrossArray(mother.weight, father.weight, child.weight, SPRM_LEN);
    return child;
}

// shift the value from 0.05 to 0.10
// no limit
void shiftMutNoLim(Sprm *prp) {
    float shift;
    for (int i = 0; i < SPRM_LEN; i++) {
        // mutation?
        if (randFloat() < MUT_RATE) {
            // from 0.05 to 0.10
            shift = randFloat() / 20 + 0.05f;
            //printFloat(shift);
            // positive
            if (rand() & 1) {
                prp->weight[i] += shift;
            } // negative
            else {
                prp->weight[i] -= shift;
            }
        }
    }
}

// shift the value from 0.05 to 0.10
// limit the value between -0.5 and 0.5
void shiftMutLimited(Sprm *prp) {
    float shift;
    for (int i = 0; i < SPRM_LEN; i++) {
        // mutation?
        if (randFloat() < MUT_RATE) {
            // from 0.05 to 0.10
            shift = randFloat() / 20 + 0.05f;
            //printFloat(shift);
            // positive
            if (rand() & 1) {
                prp->weight[i] += shift;
                // rounding
                if (prp->weight[i] > 0.5f) {
                    prp->weight[i] = 0.5f;
                }
            } // negative
            else {
                prp->weight[i] -= shift;
                // rounding
                if (prp->weight[i] < -0.5f) {
                    prp->weight[i] = -0.5f;
                }
            }
        }
    }
}

// 一定確率で乱数を返す
// mutate with a given probability
// otherwise copy
float copyOrMutation(float x, float mut_rate) {
    // equal is not necessary
    if (randFloat() <= mut_rate) {
        // from -0.5 to 0.5
        //printf("mutated!\n");
        return randFloat() - 0.5f;
    }
    return x;
}

// single point crossover
// give 2 parameters, children's array, and mutation rate
// the number of children is 2
void singlePointCrossover(Sprm mother, Sprm father, Sprm children[2], float mut_rate) {
    // a random number from 0 to 8
    int p = rand() % (SPRM_LEN - 1);
    //printDecimal(p);
    // from 0 to p
    for (int i = 0; i <= p; i++) {
        children[0].weight[i] = copyOrMutation(mother.weight[i], mut_rate);
        children[1].weight[i] = copyOrMutation(father.weight[i], mut_rate);
    }
    // from p + 1 to 9
    for (int i = p + 1; i < SPRM_LEN; i++) {
        children[0].weight[i] = copyOrMutation(father.weight[i], mut_rate);
        children[1].weight[i] = copyOrMutation(mother.weight[i], mut_rate);
    }
}

// caluculate point
float evaluationSimple(Board b, Sprm pr) {
    float pt = 0;
    int i;
    // board array
    int ba[MASU_NUM];
    // Board -> int array
    board2arraySymmetry(b, ba);
    //showDecimalArray(ba, MASU_NUM);
    for (i = 0; i < MASU_NUM; i++) {
        // calculate the inner product
        pt += ba[i] * pr.weight[CORR_TABLE[i]];
    }
    return pt;
}

// assume that the next turn is black
// n: the number of next boards
// use simple parameter
Board getBestBoardForBlackSimple(const Board *next_boards, int n, const Sprm *prp) {
    float mx_point = -FLT_MAX;
    float t_point;
    Board best_board;
    for (int i = 0; i < n; i++) {
        // sign inversion!!
        t_point = -evaluationSimple(next_boards[i], *prp);
        if (mx_point < t_point) {
            // update
            mx_point = t_point;
            best_board = next_boards[i];
        }
    }
    return best_board;
}

// assume that the next turn is black
// n: the number of next boards
// use simple parameter
// decide next board by roulette
Board getBoardForBlackSimpleRoulette(const Board *next_boards, int n, const Sprm *prp) {
    float exp_points[n];
    float s;
    int choosed;
    for (int i = 0; i < n; i++) {
        // evaluate all next boards
        // and calculate the power of e (to make numbers positive)
        // sign inversion!
        exp_points[i] = expf(-evaluationSimple(next_boards[i], *prp) * 10);
    }
    s = sumFloat(exp_points, n);
    choosed = rouletteFloat(exp_points, n, s);
    return next_boards[choosed];
}

// return winner
// give a function pointer as an argument
int oneToOneNormalSprmFlex(decNxtSprm dnfunc, const Sprm *spp, const Sprm *gpp) {
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
            main_board = dnfunc(nba, n, spp);
        } // white (second)
        else {
            //printf("white\n");
            main_board = dnfunc(nba, n, gpp);
        }
        // switch turn
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

// play against random
// return winner
// 引数で指し手決定関数を変更可能にした
// pythonで扱うときにマクロをいちいち変更するのが面倒だった
int SprmVSRandomNormal(decNxtSprm dnfunc, const Sprm *prp, int my_color) {
    Board nba[NEXT_MAX];
    int kc[3];
    int pass = 0;
    int n, dif;
    // set initial turn (black)
    int turn = 0b01;
    // set initial board
    Board main_board = START;
    while (1) {
        // calculate next (and normalize)
        n = nextBoardNormal2(main_board, nba, kc);
        // can't put a piece anywhere
        if (n == 0) {
            // can't do anything one another
            if (pass) {
                break;
            }
            // pass
            swapNormalizeBoard(&main_board);
            turn ^= 0b11;
            pass = 1;
            continue;
        }
        pass = 0;
        // determine a next board
        // parameter's turn
        if (turn == my_color) {
            main_board = dnfunc(nba, n, prp);
        } // random turn
        else {
            // randomly choose a next board
            main_board = nba[rand() % n];
        }
        // switch turn
        turn ^= 0b11;
    }
    // difference between black and white
    dif = kc[1] - kc[2];
    if (dif > 0) return turn;
    if (dif < 0) return turn ^ 0b11;
    // draw
    return 0;
}

// 指し手決定関数はマクロで指定
// calculate win rate when playing against random AI
// n: number of games
float calcWinRateSprmVSRand(Sprm pr, int pr_color, int n) {
    int count = 0;
    for (int i = 0; i < n; i++)
        // ここでマクロを使う
        if (SprmVSRandomNormal(DET_FUNC, &pr, pr_color) == pr_color)
            count++;
    return (float)count / n;
}

// ランダムAIと対戦したときの勝率を返す
// n: 白黒それぞれでの対戦数
// ついでに表示もする
// calculate win rate when playing against random AI
// n: number of games in each color
float calcWinRateSprmVSRandTotal(Sprm pr, int n) {
    float rb, rw, rt;
    rb = calcWinRateSprmVSRand(pr, 0b01, n);
    rw = calcWinRateSprmVSRand(pr, 0b10, n);
    rt = (rb + rw) / 2;
    // 各種数値を表示
    printf("win rate (black, white, total): %.2f, %.2f, %.2f\n", rb, rw, rt);
    return rt;
}

// make first generation file
// give a file name format
// record all individuals!!
int makeFirstGeneFileFlex(const char *format) {
    char fnamew[FILENAME_MAX];
    snprintf(fnamew, FILENAME_MAX, format, 0);
    // check existence
    if (warnOverwriting(fnamew) < 0)
        return -1;
    // make new family randomly
    Sprm pra[POPULATION];
    for (int i = 0; i < POPULATION; i++)
        randSprm(pra + i);
    // write
    if (dumpSprmFileDirect(fnamew, pra, sizeof pra) < 0)
        return -1;
    printf("%ld bytes were written\n", sizeof pra);
    return 0;
}

// 圧縮版Sprm初期世代ファイルを作成したい
// Sprm を経由せず直接書き込もうか
int makeFGFileSprmComp(const char *format) {
    char fnamew[FILENAME_MAX];
    snprintf(fnamew, FILENAME_MAX, format, 0);
    warnOverwritingExit(fnamew);
    u_char uca[SPRM_FILE_SIZE_COMP];
    // ただの符号無文字型乱数配列を作成
    randUcharArray(uca, SPRM_FILE_SIZE_COMP);
    // 末尾はフラグ0で初期化
    uca[SPRM_FILE_SIZE_COMP - 1] = 0;
    // そのまま書き込み
    dumpFileDirectExit(fnamew, uca, SPRM_FILE_SIZE_COMP);
    // 初期世代作成時のみサイズを表示
    printf("%d bytes were written\n", SPRM_FILE_SIZE_COMP);
    return 0;
}

// ファイル名をそのまま与えてSprmをロード
int loadSprmFileDirect(const char *fname, Sprm *pra, size_t pra_size) {
    FILE *fp;
    // 読み込み用で開く
    if ((fp = fopen(fname, "rb")) == NULL) {
        // 失敗
        printf("%s can't be opened.\n", fname);
        return -1;
    }
    // 成功
    fread(pra, pra_size, 1, fp);
    fclose(fp);
    return 0;
}

// 圧縮ファイルからロード
// エラーやソート済みフラグを返す
// 返り値が重要な場合はマクロ化しにくいかな?
// 個体数を指定するように変更
int loadSprmFileCompDirect(const char *fname, Sprm *pra, int loc_pop) {
    const int loc_size = SPRM_LEN * loc_pop + 1;
    u_char uca[loc_size];
    // ロードかエラー
    loadFileDirectExit(fname, uca, loc_size);
    // 展開
    extrSprmArray(uca, pra, loc_pop);
    // フラグを返す
    return (int)(uca[loc_size - 1]);
}

// read parameters from a file
int loadSprmFile(const char *format, int gene_num, Sprm *pra, size_t pra_size) {
    // the file name to be read
    char fnamer[FILENAME_MAX];
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    // ファイル名確認
    printf("read file : %s\n", fnamer);
    // ロードしてエラー値を返す
    return loadSprmFileDirect(fnamer, pra, pra_size);
}

// load a representative of Sprm
Sprm loadRepSprm(const char *format, int gene_num, int loc_pop) {
    Sprm pra[loc_pop];
    if (loadSprmFile(format, gene_num, pra, sizeof pra) < 0) {
        // failed
        // set random parameters
        randSprm(pra);
    }
    // return the top parameter
    return *pra;
}

// write parameters to a file
// give a file name for writing
// be careful of overwriting
int dumpSprmFileDirect(const char *fname, Sprm *pra, size_t pra_size) {
    FILE *fp;
    if ((fp = fopen(fname, "wb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fname);
        return -1;
    }
    fwrite(pra, pra_size, 1, fp);
    fclose(fp);
    return 0;
}

// check parameter in a file
// give the file name format and generation number
void checkSprmFile(const char *format, int gene_num) {
    Sprm pra[POPULATION];
    if (loadSprmFile(format, gene_num, pra, sizeof pra) < 0) {
        // failed
        return;
    }
    // check some parameters
    showFamilyPart(pra);
}

// use Sprm[100]
// win: +2, draw: +1, lose: 0
// give a function pointer to decide the next board
void leagueMatchSprmFlex(decNxtSprm dnfunc, const Sprm *generation, int *result) {
    // set all elements to zero
    zeros(result, POPULATION);
    // black index
    for (int i = 0; i < POPULATION; i++) {
        //printf("\ai = %d / %d", i, FAMILY_NUM);
        // white index
        for (int j = 0; j < POPULATION; j++) {
            if (i == j) continue;
            switch(oneToOneNormalSprmFlex(dnfunc, generation + i, generation + j)) {
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

// calculate distance
float distSprm(Sprm p1, Sprm p2) {
    int i;
    float d = 0.0;
    for (i = 0; i < SPRM_LEN; i++) {
        // add square distance
        d += (float)pow(p1.weight[i] - p2.weight[i], 2.0);
    }
    // divided by number of parameters
    d /= SPRM_LEN;
    // return the square root
    return (float)sqrt(d);
}

// 各値ごと平均値と標準偏差を計算して表示
// nos はサンプル数の意味
// calculate means and standard deviation from some parameters
void checkSprmStatistics(const Sprm *pra, int nos) {
    int i, j;
    float mean[SPRM_LEN];
    float sd[SPRM_LEN];
    zerosFloat(mean, SPRM_LEN);
    zerosFloat(sd, SPRM_LEN);
    // calculate sum of each weight
    for (i = 0; i < nos; i++)
        for (j = 0; j < SPRM_LEN; j++)
            mean[j] += pra[i].weight[j];
    
    // divide by the number of samples
    for (j = 0; j < SPRM_LEN; j++)
        mean[j] /= nos;
    
    // calculate sum of the square of each weight
    for (i = 0; i < nos; i++)
        for (j = 0; j < SPRM_LEN; j++)
            sd[j] += square(pra[i].weight[j]);
    
    // calculate standard deviation
    for (j = 0; j < SPRM_LEN; j++)
        sd[j] = sqrtf(sd[j] / nos - square(mean[j]));
    
    // display
    printString("statistics:");
    printf("mean: ");
    printFloatArray(mean, SPRM_LEN);
    printf("SD:   ");
    printFloatArray(sd, SPRM_LEN);
}

// choose survivors from Sprm[100]
// and show match results
void getSurvivorSprm(Sprm *generation, Sprm *survivors) {
    int i, j;
    int result[POPULATION];
    int number[POPULATION];
    float dist;
    // number = {0, 1, 2, ..., 99}
    for (i = 0; i < POPULATION; i++)
        number[i] = i;
    // game!
    leagueMatchSprmFlex(DET_FUNC, generation, result);
    // sort (descending order)
    quicksortDD(result, number, 0, POPULATION - 1);
    // show ranking
    printf("rank change\n");
    for (i = 0; i < POPULATION; i++) {
        // rank 11 .. 99 aren't displayed
        if (10 <= i && i < POPULATION - 1) continue;
        // worst!
        if (i == POPULATION - 1)
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
    dist = distSprm(survivors[0], generation[0]);
    printf("distance: %6.4f\n", dist);
    //printf("top parameters view:\n");
    //showSprm(survivors[0]);
}

// make next generation file
// give a function to choose survivors
int nextGenerationSprmFlex(void (*getSvr)(const Sprm*, Sprm*), const char *format, int gene_num, int safety) {
    int i, j, count;
    char fnamer[FILENAME_MAX], fnamew[FILENAME_MAX];
    // previous survivors
    Sprm parents[SURVIVE_NUM];
    FILE *fp;
    // the file name to be read
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    // the file name to be written
    snprintf(fnamew, FILENAME_MAX, format, gene_num + 1);
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
    Sprm generation[POPULATION];

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
    // battle!
    getSvr(generation, survivors);

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

// ルーレット選択, 一様交叉, ランダム突然変異
// ソート済み前提のため個体番号は使わない
void rltUniRdS(const int *fitness, const Sprm *current, Sprm *next) {
    int parents[2];
    for (int count = ELITE_NUM; count < POPULATION; count++) {
        rouletteIntMltDep(fitness, POPULATION, parents, 2);
        next[count] = makeChildCrossMSprm(current[parents[0]], current[parents[1]]);
    }
}

// 選択・交叉・突然変異の関数は別途定義
// make next generation file
// write all individuals to the file
// give a function pointer for selection and crossing
int nGeneSSAFlex(scmFunc selAndCross, const char *format, int gene_num, int safety) {
    char fnamew[FILENAME_MAX];
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
    // evaluate fitness
    // the function to determine the next board is defined in "DET_FUNC"
    leagueMatchSprmFlex(DET_FUNC, current, fitness);
    // sort (descending order)
    randomizedQuicksortDDAll(fitness, numbers, POPULATION);
    // show the part of fitness
    printString("results:");
    printDecimalArrayPart(fitness, POPULATION);
    // elite selection
    for (int i = 0; i < ELITE_NUM; i++)
        next[i] = current[numbers[i]];
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

// give a function to loop
void nextGenerationSprmLoopFlex(int (*nGene)(int, int), int safety, int st, int loop) {
    time_t t0, t1;
    // get start time
    time(&t0);
    for (int i = st; i < st + loop; i++) {
        // set the generation number as the seed
        srand((unsigned)i);
        if (nGene(i, safety) == -1)
            return;
        // get time
        time(&t1);
        printf("elapsed time: %lds\n", t1 - t0);
    }
}

// give a function to loop and file name format
void nGeneSSALoopFlex(int (*nGeneSSA)(const char*, int, int), const char *format, int safety, int st, int loop) {
    time_t t0, t1;
    // get start time
    time(&t0);
    for (int i = st; i < st + loop; i++) {
        // set the generation number as the seed
        srand((unsigned)i);
        if (nGeneSSA(format, i, safety) < 0)
            // error
            return;
        // get time
        time(&t1);
        printf("elapsed time: %lds\n", t1 - t0);
    }
}

// give a function pointer for selection and crossover
void nGeneSSAFlexLoop(scmFunc selAndCross, const char *format, int safety, int st, int loop) {
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
        kugiri(100);
    }
}

// give a function pointer for selection and crossover
// use SEED
void nGeneSSAFlexLoopSeed(scmFunc selAndCross, const char *format, int safety, int st, int loop) {
    time_t t0, t1;
    // get start time
    time(&t0);
    // 代表者
    Sprm rep_pra;
    unsigned int s1, s2;
    for (int i = st; i < st + loop; i++) {
        // 無駄?にシード設定しまくる部分
        s1 = i + SEED;
        printf("seed1: %d\n", s1);
        // set seed
        srand(s1);
        s2 = rand() + SEED;
        printf("seed2: %d\n", s2);
        // set seed
        srand(s2);
        if (nGeneSSAFlex(selAndCross, format, i, safety) < 0) {
            // error
            printString("error");
            return;
        }
        // 戦績チェック (参考用)
        // 見るのはソートされていない次世代の先頭要素
        // 実質的には現世代のトップになるはず
        // 次の世代作成の乱数が変わらないように場所に注意
        if (!((i + 1) % 20)) {
            kugiri(100);
            rep_pra = loadRepSprm(format, i + 1, POPULATION);
            printf("the strongest:\n");
            calcWinRateSprmVSRandTotal(rep_pra, 500);
        }
        // get time
        time(&t1);
        printf("elapsed time: %lds\n", t1 - t0);
        kugiri(100);
    }
}

// give a function to loop
void nextGenerationSprmFlexLoopFlex(void (*getSvr)(const Sprm*, Sprm*), int (*nGeneF)(void(), const char*, int, int), const char *format, int safety, int st, int loop) {
    time_t t0, t1;
    // get start time
    time(&t0);
    for (int i = st; i < st + loop; i++) {
        // set the generation number as the seed
        srand((unsigned)i);
        if (nGeneF(getSvr, format, i, safety) == -1)
            return;
        // get time
        time(&t1);
        printf("elapsed time: %lds\n", t1 - t0);
    }
}

// copy the first generation
// give the destination file format
void copyFGFlex(const char *dst_format) {
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
    // check the parameters
    for (int i = 0; i < SURVIVE_NUM; i++)
        printFloatArray(pa[i].weight, SPRM_LEN);
    // file name for writing (destination)
    char fnamew[FILENAME_MAX];
    snprintf(fnamew, FILENAME_MAX, dst_format, 0);
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

// 圧縮されたファイルから個体を読み出し, 適応度を評価する
// 適応度降順に個体を並び替え, 同じファイルに書き込む
// 適応度はルーレット選択等に用いるため, 呼び出し元で配列を渡す
// 次世代作成関数が膨らまないように, ここで適応度の読み書きをする
int sortSprmCompFileByFitness(const char *fname, int *fitness) {
    // ソート前とソート後のパラメータ配列を用意する (メモリの無駄遣いかな?)
    Sprm pra1[POPULATION], pra2[POPULATION];
    // 適応度ファイル名
    char fnamef[FILENAME_MAX];
    // 適応度ファイル名作成, オーバーフローしたら抜ける
    makeFitnessFileNameDirectExit(fnamef, FILENAME_MAX, fname);
    // ロードしてフラグを取得
    int flag = loadSprmFileCompDirect(fname, pra1, POPULATION);
    // エラーなら-1を返す
    if (flag < 0) return -1;
    // ソート済みなら適応度ファイルを読み込む
    if (flag == 1) {
        // 読み込めなかったらエラーを返す
        loadFitnessShortDirectExit(fnamef, fitness, POPULATION);
        // ソート済みフラグを返す
        return 1;
    }
    // 個体番号を割り振る
    int numbers[POPULATION];
    indices(numbers, POPULATION);
    // リーグ戦 (指し手決定関数はマクロ依存)
    // 現状で適応度評価はこれだけ
    leagueMatchSprmFlex(DET_FUNC, pra1, fitness);
    // 適応度を基に個体番号も同時にソート
    randomizedQuicksortDDAll(fitness, numbers, POPULATION);
    // 適応度順に並び替えてpra2に代入
    for (int i = 0; i < POPULATION; i++)
        pra2[i] = pra1[numbers[i]];
    // ソート後の配列を同じファイルに書き戻す (ソート済みフラグを立てる)
    dumpSprmFileCompDirectExit(fname, pra2, 1);
    // 適応度書き込み
    dumpFitnessShortDirectExit(fnamef, fitness, POPULATION)
    return 0;
}

// 次の世代のファイルを作る関数 (圧縮バージョン)
// ついでに適応度評価をした現世代のファイルもソートして書き換える
// ソート済みファイルを使ってルーレット選択をする際, 適応度も必要と考えてファイルに保存
// 再現性確保のためのシードを2つ与える
// 個体番号配列を撤廃
int nGeneSprmComp(scmSprmSorted scm, const char *format, int gene_num, u_int seed1, u_int seed2, int safety) {
    // 読み込み (ソート) 用と書き込み用ファイル名
    char fnames[FILENAME_MAX], fnamew[FILENAME_MAX];
    snprintf(fnames, FILENAME_MAX, format, gene_num);
    snprintf(fnamew, FILENAME_MAX, format, gene_num + 1);
    printf("sort file: %s\n", fnames);
    printf("new file : %s\n", fnamew);
    // 上書き拒否
    if (safety) warnOverwritingExit(fnamew);
    int fitness[POPULATION];
    // 対戦・ソート用シード
    srand(seed1);
    // 適応度評価とファイルのソート
    // さらに適応度ファイルの読み書きも行う
    int flag = sortSprmCompFileByFitness(fnames, fitness);
    // エラー
    if (flag < 0) return -1;
    // ソート済み
    if (flag == 1) {
        printf("%s is already sorted!\n", fnames);
    }
    // 選択・交叉・突然変異用シード
    srand(seed2);
    // なんとなくソート済み適応度を表示
    printDecimalArray(fitness, POPULATION);
    // 今世代と次世代の個体配列を宣言
    Sprm current[POPULATION], next[POPULATION];
    // ソート済み配列を読み込む
    if (loadSprmFileCompDirect(fnames, current, POPULATION) < 0) {
        return -1;
    }
    // エリートはそのままコピー
    copyArray(current, next, ELITE_NUM);
    // 選択, 交叉, 突然変異
    scm(fitness, current, next);
    // 現世代の個体の一部と統計値を確認
    showFamilyPart(current);
    checkSprmStatistics(current, POPULATION);
    // 乱数に影響が出ないように次世代を作ったら勝率計算 (たまーに)
    // ループ関数じゃなくてこっちでやった方がロードの手間は少ない
    if (!(gene_num % 20)) {
        kugiri(100);
        int game_num = 500;
        printf("the number of games: %d x 2\n", game_num);
        printf("the strongest:\n");
        calcWinRateSprmVSRandTotal(current[0], game_num);
        printf("the weakest:\n");
        calcWinRateSprmVSRandTotal(current[POPULATION - 1], game_num);
    }
    // ソート済みフラグは立てずに書き込み
    dumpSprmFileCompDirectExit(fnamew, next, 0);
    return 0;
}

// 圧縮版次世代作成関数をループさせる関数
// 引数は開始世代番号と, 終了世代番号に変更 (最終世代はファイル作成のみ)
void nGeneSprmCompLoop(scmSprmSorted scm, const char *format, int safety, int start, int stop) {
    time_t t_arr[2];
    // 初期時刻を記録
    time(t_arr);
    u_int s1, s2;
    for (int gene_num = start; gene_num < stop; gene_num++) {
        // 1 はなんとなくのオフセット
        srand(s1 = SEED + gene_num + 1);
        // s2はs1に依存する
        // 排他的論理和で乱数改変
        // 和を計算したときのオーバーフローを回避?
        printf("seed1: %d, seed2: %d\n", s1, s2 = rand() ^ SEED);
        // 次の世代へ!
        if (nGeneSprmComp(scm, format, gene_num, s1, s2, safety))
            return;
        // get time
        time(t_arr + 1);
        printf("elapsed time: %lds\n", t_arr[1] - t_arr[0]);
        kugiri(100);
    }
}
