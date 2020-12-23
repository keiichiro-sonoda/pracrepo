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

// 適応度評価関数 (マクロのIDによって決まる)
efSprm EF_FUNC_SPRM;

// シードが正しいか確認用
int FIRST_RAND;

// 有効になっている公比
double CMN_RATIO_EFF;

// functions

// グローバル変数等初期化関数
// シード設定も行う
// パラメータも表示
void initSprm(void) {
    srand(SEED);
    FIRST_RAND = rand();
    srand(SEED);
    setCORR_TABLE();
    initBoard();
    makeSprmSample();
    printf("population          : %4d\n", POPULATION);
    printf("the number of elites: %4d\n", ELITE_NUM);
    printf("mutation rate       : %4.2f\n", MUT_RATE);
    printf("parameter file size : %4d\n", SPRM_FILE_SIZE_COMP);
    printf("seed                : %4d\n", SEED);
    printf("適応度評価方法      : ");
    // 2bit目を見る
    switch ((EF_FUNC_ID >> 1) & 1) {
        case 0: // リーグ戦
            EF_FUNC_SPRM = leagueMatchSprmFlex;
            printf("リーグ戦");
            break;
        default:
            EF_FUNC_SPRM = evalFitnessSprmVSRandFGN;
            printf("対ランダム");
    }
    // 1bit目を見る
    switch (EF_FUNC_ID & 1) {
        case 0: // 常に評価が最大の手を選ぶ
            DET_FUNC = getBestBoardForBlackSimple;
            printf(" (指し手マックス)\n");
            break;
        default: // ルーレットで手を選ぶ
            DET_FUNC = getBoardForBlackSimpleRoulette;
            printf(" (指し手ルーレット)\n");
    }
}

// Sprm のファイルフォーマットを自動生成する関数
// 適応度ID, 圧縮判定, 個体数, エリート数, 選択ID, 交叉ID, 突変ID, 突変率, シード値, その他
int makeSprmFileFormatAuto(char *dst, int dst_size, int eff_id, int is_comp, int loc_pop, int loc_eln, int sel_id, int crs_id, int mut_id, double loc_mr, int loc_seed, ...) {
    // 引数の構造体?
    va_list args;
    // 開始. 第二引数には最後の固定引数?を渡すらしい
    va_start(args, loc_seed);
    // s\0 で初期化した文字列. ここにシード以外の情報が入る?
    char info_str[BUF_LEN] = {115, 0};
    char tmp_str[BUF_LEN];
    // 可変引数に代入された値を取り出す
    int arg_i;
    double arg_d;
    // 適応度
    switch (eff_id & 0b10) {
        case 0b00: // リーグ戦
            strcatSize(info_str, "l", BUF_LEN);
            break;
        default: // 対ランダム
            strcatSize(info_str, "rd", BUF_LEN);
    }
    switch (eff_id & 0b01) {
        case 0b00:// 指し手固定
            strcatSize(info_str, "d", BUF_LEN);
            break;
        default: // 指し手ルーレット
            strcatSize(info_str, "r", BUF_LEN);
    }
    // 圧縮するかどうか
    if (is_comp) strcatSize(info_str, "c", BUF_LEN);
    // 個体数は3桁
    snprintf(tmp_str, BUF_LEN, "%03d_", loc_pop);
    strcatSize(info_str, tmp_str, BUF_LEN);
    // エリート数は2桁 (0なら表示しない)
    if (loc_eln) {
        snprintf(tmp_str, BUF_LEN, "%02d", loc_eln);
        strcatSize(info_str, tmp_str, BUF_LEN);
    }
    strcatSize(info_str, "_", BUF_LEN);
    // 選択
    switch (sel_id) {
        case 2: // 等比数列ランキング選択
            // 可変長引数で公比が与えられるはずなので, 100倍した整数値を最低2桁で表示
            snprintf(tmp_str, BUF_LEN, "rkg%02d", (int)(va_arg(args, double) * 100));
            // マクロをダイレクトに公比として使う
            CMN_RATIO_EFF = CMN_RATIO;
            strcatSize(info_str, tmp_str, BUF_LEN);
            break;
        case 3: // 等比数列ランキング選択 (自然対数表記型)
            // 可変長引数で公比の自然対数が与えられるとし, 符号付きで1の位から小数第3位まで表示
            if ((arg_d = va_arg(args, double)) == 0.) {
                // 0.0 は + に統一
                snprintf(tmp_str, BUF_LEN, "rkgexp%+6.3f", .0);
            } else {
                snprintf(tmp_str, BUF_LEN, "rkgexp%+6.3f", arg_d);
            }
            // ここで公比を決定する
            CMN_RATIO_EFF = exp(arg_d);
            //printFloatExp(arg_d);
            strcatSize(info_str, tmp_str, BUF_LEN);
            break;
        default:
            miteigiExit(-1);
    }
    strcatSize(info_str, "_", BUF_LEN);
    // 交叉
    switch (crs_id) {
        case 5: // 一様交叉 (2人っ子)
            strcatSize(info_str, "uni2", BUF_LEN);
            break;
        case 6: // BLX-α 交叉
            arg_d = va_arg(args, double);
            snprintf(tmp_str, BUF_LEN, "blxa%4.2f", arg_d);
            strcatSize(info_str, tmp_str, BUF_LEN);
            break;
        default:
            miteigiExit(-1);
    }
    // 突然変異 (突然変異率が0より大きい場合のみ処理する)
    if (loc_mr > 0) {
        strcatSize(info_str, "_", BUF_LEN);
        switch (mut_id) {
            case 0: // ランダム突然変異
                snprintf(tmp_str, BUF_LEN, "rd%03d", (int)(loc_mr * 100));
                strcatSize(info_str, tmp_str, BUF_LEN);
                break;
            default:
                miteigiExit(-1);
        }
    }
    // 初期世代雑魚フラグ
    if ((arg_i = va_arg(args, int)) == 1) {
        strcatSize(info_str, "_zako", BUF_LEN);
    }
    // パスを作成 (ディレクトリ名とファイル名の共通部分が info_str)
    snprintf(dst, dst_size, "prm/%s/%s", info_str, info_str);
    strcatSize(dst, "_g%03d", dst_size);
    // シードと拡張子の追加
    snprintf(tmp_str, BUF_LEN, "_s%03d.bin", loc_seed);
    strcatSize(dst, tmp_str, BUF_LEN);
    // 終了. これが必要らしい, メモリの解放?
    va_end(args);
    return 0;
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
void showFamilyPart(const Sprm *pra) {
    // population is 5 or less
    if (POPULATION <= 5) {
        // show all
        for (int i = 0; i < POPULATION; i++) {
            // 番号も表示
            printf("%4d: ", i);
            showSprmOneLine(pra[i]);
        }
        return;
    }
    // population is greater than 5
    for (int i = 0; i < 3; i++) {
        printf("%4d: ", i);
        showSprmOneLine(pra[i]);
    }
    printf("                                           ...\n%4d: ", POPULATION - 1);
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
// ランダム突然変異を含む
void singlePointCrossover(Sprm mother, Sprm father, Sprm children[2], float mut_rate) {
    // a random number from 0 to 8
    int p = randInt(SPRM_LEN - 1);
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

// 一様交叉 (2人っ子), 突然変異なし
void uniCrossSprm2C(const Sprm *mother, const Sprm *father, Sprm children[2]) {
    int rb;
    for (int i = 0; i < SPRM_LEN; i++) {
        // 0 か 1 を返す
        rb = randBit();
        // 添字が rb と一致する子は母から受け継ぐ
        children[rb].weight[i] = mother->weight[i];
        // 添字が rb でない子は父から受け継ぐ
        children[rb ^ 1].weight[i] = father->weight[i];
    }
}

// BLX-α 交叉? (Blend Crossover)
// 2つの個体でできる超直方体の内部もしくは周辺に子を作成
// 子は1回につき1つ (連続専用交叉で乱数多め?)
// 圧縮対応の値に変換する
Sprm blendCrossSprmComp(const Sprm *mother_p, const Sprm *father_p) {
    Sprm child;
    float tmp, min_a, max_a, d;
    for (int i = 0; i < SPRM_LEN; i++) {
        // 代入しつつ比較
        if ((min_a = mother_p->weight[i]) > (max_a = father_p->weight[i])) {
            // 母の値が大きければ交換
            swap(float, min_a, max_a);
        }
        // 差を計算
        d = max_a - min_a;
        // 差に比例した倍率で範囲を少し引き伸ばす
        max_a += d * ALPHA_BLX;
        min_a -= d * ALPHA_BLX;
        // 範囲内の一様乱数を作成 (なぜか double 型)
        tmp = randDoubleRange(min_a, max_a);
        // 値を範囲内に収める
        tmp = clamp(tmp, -0.5f, 0.5f);
        // 一度圧縮して元に戻すことで, 値のズレを防止
        // 交叉直後にファイル書き込みする場合は正直不要
        child.weight[i] = uchar2weight(weight2uchar(tmp));
    }
    return child;
}

// 交叉テスト用関数
void crossTestSprm(void) {
    //srand((unsigned)time(NULL));
    Sprm pr1, pr2, pra[2];
    initArrayConst(pr1.weight, SPRM_LEN, -.000f);
    initArrayConst(pr2.weight, SPRM_LEN, .100f);
    printf("親1: ");
    showSprmOneLine(pr1);
    printf("親2: ");
    showSprmOneLine(pr2);
    //uniCrossSprm2C(&pr1, &pr2, pra);
    pra[0] = blendCrossSprmComp(&pr1, &pr2);
    printf("子1: "); showSprmOneLine(pra[0]);
    //printf("子2: "); showSprmOneLine(pra[1]);
}

// Sprm で盤面を評価
// caluculate point
float evaluationSimple(Board b, Sprm pr) {
    float pt = 0;
    int i, ba[MASU_NUM];
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
    // 初期盤面の変更に伴い正規化が必要になった
    Board main_board = normalBoard(START);
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
    Board main_board, nba[NEXT_MAX];
    int winner, n, dif, pass, turn, kc[3];
    pass = 0;
    // set initial turn (black)
    turn = 0b01;
    // set initial board
    // 正規化を忘れずに
    main_board = normalBoard(START);
    while (1) {
        //showBoard(main_board);
        // calculate next (and normalize)
        // can't put a piece anywhere
        if ((n = nextBoardNormal2(main_board, nba, kc)) == 0) {
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
            //printf("Sprm のターン: %d\n", turn);
            main_board = dnfunc(nba, n, prp);
        } // random turn
        else {
            // randomly choose a next board
            //printf("ランダムのターン: %d\n", turn);
            main_board = nba[randInt(n)];
        }
        // switch turn
        turn ^= 0b11;
    }
    // 最後に指した手番が turn に記録される
    // 最後に指した人のコマは黒(◯)で終了する
    // 黒の方が多ければ最後に指した方が勝利
    // 白の方が多ければもう一方が勝利
    // difference between black and white
    if ((dif = kc[1] - kc[2]) > 0) {
        winner = turn;
    } else if (dif < 0) {
        winner = turn ^ 0b11;
    } else {
        winner = 0;
    }
    return winner;
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
    // 警告と退出 (この関数自体使われるのか?)
    warnOverwritingExit(fnamew);
    // make new family randomly
    Sprm pra[POPULATION];
    for (int i = 0; i < POPULATION; i++)
        randSprm(pra + i);
    // 書き込みと退出
    dumpFileDirectExit(fnamew, pra, sizeof pra);
    printf("%ld bytes were written\n", sizeof pra);
    return 0;
}

// 上書き注意をせず, ファイル名直接指定
// Sprm を経由せず直接書き込む
int _makeFGFileSprmComp(const char *fnamew) {
    u_char uca[SPRM_FILE_SIZE_COMP];
    // シードセット!
    srand(SEED);
    // ただの符号無文字型乱数配列を作成
    randUcharArray(uca, SPRM_FILE_SIZE_COMP);
    // 末尾はフラグ0で初期化
    uca[SPRM_FILE_SIZE_COMP - 1] = 0;
    // そのまま書き込み, 失敗したら -1 で抜ける
    dumpFileDirectExit(fnamew, uca, SPRM_FILE_SIZE_COMP);
    // 初期世代作成時のみサイズを表示
    printf("%d bytes were written\n", SPRM_FILE_SIZE_COMP);
    return 0;
}

// 圧縮版Sprm初期世代ファイルを作成
// マクロ SEED でシード設定してファイル名とシードを対応させる
// ファイル名が正しく設定されていることは前提だが
int makeFGFileSprmComp(const char *format) {
    puts("初期世代作成");
    char fnamew[FILENAME_MAX];
    snprintf(fnamew, FILENAME_MAX, format, 0);
    warnOverwritingExit(fnamew);
    // 別関数に委託 (失敗したら -1)
    return _makeFGFileSprmComp(fnamew);
}

// ファイル名からディレクトリのパスを抽出 (ファイル直下)
int extractDirPath(const char *fname, char *dir, int dir_size) {
    int i, l;
    l = strlen(fname);
    char fname_cp[l + 1];
    strncpy(fname_cp, fname, l);
    for (i = l - 1; i >= 0; i--) {
        // '/' が見つかったらなる文字に置換
        // i はナル文字の位置を示して抜ける
        if (fname_cp[i] == '/') {
            fname_cp[i] = 0;
            break;
        }
    }
    // '/' がファイル名に含まれていない, もしくはオーバーフロー
    if (i < 0 || dir_size < i) {
        puts("ディレクトリ名抽出失敗");
        return -1;
    }
    snprintf(dir, dir_size, "%s", fname_cp);
    return 0;
}

// 初期世代作成, 必要に応じてディレクトリを作成
int makeFGFileSprmCompMkdir(const char *format, int safety) {
    puts("初期世代作成");
    FILE *fp;
    char fnamew[FILENAME_MAX];
    snprintf(fnamew, FILENAME_MAX, format, 0);
    // 読み込み専用で開けた
    if ((fp = fopen(fnamew, "rb"))) {
        fclose(fp);
        printf("\a\"%s\" は存在します. ", fnamew);
        // 安全装置が -1 の場合は特殊フラグで抜ける
        if (safety == -1) {
            // 改行だけする
            putchar(10);
            return -2;
        }
        // 上書き警告, 許可が得られなければ抜ける
        printf("上書きしますか?");
        kakuninExit();
    }
    // 読み込み失敗
    // ファイルが存在しない? ので上書きを気にせず書き込み専用で開く
    else if ((fp = fopen(fnamew, "wb"))) {
        fclose(fp);
    }
    // 書き込みも失敗
    else {
        char new_dir[FILENAME_MAX];
        // ディレクトリ名抽出
        if (extractDirPath(fnamew, new_dir, FILENAME_MAX) < 0) {
            return -1;
        }
        // 安全装置が有効な場合, 許可を得る
        if (safety > 0) {
            printf("ディレクトリ %s を作成しますか?", new_dir);
            kakuninExit();
        }
        // ディレクトリ作成
        if (mkdir(new_dir, 0775) < 0) {
            puts("ディレクトリ作成失敗");
            return -1;
        }
        printf("ディレクトリ %s が作成されました\n", new_dir);
    }
    // 途中で抜けなければ初期世代作成
    return _makeFGFileSprmComp(fnamew);
}

// 圧縮ファイルからロード
// エラーやソート済みフラグを返す
// 返り値が重要な場合はマクロ化しにくいかな?
// 個体数を指定するように変更
int loadSprmFileCompDirect(const char *fname, Sprm *pra, int loc_pop) {
    const int loc_size = SPRM_LEN * loc_pop + 1;
    u_char uca[loc_size];
    // ロードか, エラーで抜け出す
    loadFileDirectExit(fname, uca, loc_size);
    // 展開
    extrSprmArray(uca, pra, loc_pop);
    // フラグを返す
    return uca[loc_size - 1];
}

// read parameters from a file
int loadSprmFile(const char *format, int gene_num, Sprm *pra, size_t pra_size) {
    // the file name to be read
    char fnamer[FILENAME_MAX];
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    // ファイル名確認
    printf("read file : %s\n", fnamer);
    // ロード. エラーなら-1で抜ける
    loadFileDirectExit(fnamer, pra, pra_size);
    return 0;
}

// 圧縮ファイルからロード (フォーマットと世代番号から)
// 返り値はエラーフラグか, ソートフラグ
int loadSprmFileComp(const char *format, int gene_num, Sprm *pra, int loc_pop) {
    char fnamer[FILENAME_MAX];
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    return loadSprmFileCompDirect(fnamer, pra, loc_pop);
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

// 圧縮ファイルからパラメータを読み取って確認
void checkSprmFileComp(const char *format, int gene_num) {
    Sprm pra[POPULATION];
    int flag = loadSprmFileComp(format, gene_num, pra, POPULATION);
    if (flag < 0) return;
    printf("確認するファイル: ");
    printf(format, gene_num);
    if (flag == 1) {
        printf("\nソート済\n");
    } else {
        printf("\n未ソート\n");
    }
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

// ランダムな手を選ぶ相手との対戦で, 適応度を決める
// 勝点の計算は今まで同様とする
// 試合数を引数で渡す以外は, リーグ戦の引数と同様
void evalFitnessSprmVSRand(decNxtSprm dnfunc, const Sprm *family, int *result, int gn) {
    int i, j, color, winner;
    // 結果配列は 0 で初期化
    zeros(result, POPULATION);
    // 個体を順番に評価
    for (i = 0; i < POPULATION; i++) {
        // 手番は黒(1)と白(2)両方
        for (color = 1; color <= 2; color++) {
            // 各個体, 各色毎に渡された試合数だけくり返す
            for (j = 0; j < gn; j++) {
                // 勝者を取得
                // 個体の勝利 (勝ち点2)
                if ((winner = SprmVSRandomNormal(dnfunc, family + i, color)) == color) {
                    result[i] += 2;
                }
                // 引き分け (勝ち点1)
                else if (winner == 0) {
                    result[i]++;
                }
                // 負けは何もしない
            }
        }
    }
}

// 試合数が固定されたバージョン (fix the nubmer of games)
// リーグ戦関数と型を合わせるため
void evalFitnessSprmVSRandFGN(decNxtSprm dnfunc, const Sprm *family, int *result) {
    evalFitnessSprmVSRand(dnfunc, family, result, GAME_NUM);
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

// Sprm の平均値を求める
// 統計値はなんとなく double 型を使う
void calcSprmMeans(const Sprm *pra, int n, double *means) {
    int i, j;
    // 0 で初期化
    zeros(means, SPRM_LEN);
    for (j = 0; j < SPRM_LEN; j++) {
        for (i = 0; i < n; i++) {
            // 各重みの合計値を計算
            means[j] += pra[i].weight[j];
        }
        // サンプル数で割る
        means[j] /= n;
    }
}

// Sprm の分散を求める (サンプル数 - 1 で割る)
// 引数には平均値も与える
void calcSprmVariances(const Sprm *pra, int n, const double *means, double *vars) {
    int i, j;
    // 0 で初期化
    zeros(vars, SPRM_LEN);
    // サンプル数 1 なら分散 0 のまま返す
    if (n < 2) return;
    for (j = 0; j < SPRM_LEN; j++) {
        for (i = 0; i < n; i++) {
            // 各重みの偏差の2乗の合計値を計算
            vars[j] += pow(pra[i].weight[j] - means[j], 2.);
        }
        // サンプル数 - 1 で割る
        vars[j] /= n - 1;
    }
}

// 各値ごと平均値と標準偏差を計算して表示
// nos はサンプル数の意味
// 負のルートを計算したくないので定義どおりの計算方法を用いる
// さらに n - 1 で割る方法に戻す
// calculate means and standard deviation from some parameters
void checkSprmStatistics(const Sprm *pra, int nos) {
    double mean[SPRM_LEN], bunsan[SPRM_LEN], sd[SPRM_LEN];
    // 平均値計算は別関数に委託
    calcSprmMeans(pra, nos, mean);
    // 分散計算も別関数に委託
    calcSprmVariances(pra, nos, mean, bunsan);
    // 平方根とって標準偏差にする
    sqrtArray(bunsan, sd, SPRM_LEN);
    // display
    printString("statistics:");
    printf("mean: ");
    printFloatArray(mean, SPRM_LEN);
    //printf("var : "); printFloatArray(bunsan, SPRM_LEN);
    printf("SD  : ");
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
// 圧縮しないバージョン
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
    // 安全装置チェック, 上書き警告, 退出
    if (safety) warnOverwritingExit(fnamew);
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
    // 書き込みか退出
    dumpFileDirectExit(fnamew, next, sizeof next);
    return 0;
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
// 適応度を計算する関数を引数で渡す
// 適応度はルーレット選択等に用いるため, 呼び出し元で配列を渡す
// 次世代作成関数が膨らまないように, ここで適応度の読み書きをする
int sortSprmCompFileByFitness(efSprm eFit, const char *fname, int *fitness) {
    // ソート前とソート後のパラメータ配列を用意する (メモリの無駄遣いかな?)
    Sprm pra1[POPULATION], pra2[POPULATION];
    // 適応度ファイル名
    char fnamef[FILENAME_MAX];
    // 適応度ファイル名作成, オーバーフローしたら抜ける
    makeFitnessFileNameDirectExit(fnamef, FILENAME_MAX, fname);
    int flag;
    // ロードしてフラグを取得
    // エラーなら-1を返す
    if ((flag = loadSprmFileCompDirect(fname, pra1, POPULATION)) < 0) return -1;
    // ソート済みなら適応度ファイルを読み込む
    if (flag == 1) {
        // 読み込めなかったら抜ける
        loadFitnessShortDirectExit(fnamef, fitness, POPULATION);
        // ソート済みフラグを返す
        return 1;
    }
    // 個体番号を割り振る
    int numbers[POPULATION];
    indices(numbers, POPULATION);
    // 適応度評価 (指し手決定関数はグローバル変数依存)
    eFit(DET_FUNC, pra1, fitness);
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
    // 上書き拒否なら抜ける
    if (safety) warnOverwritingExit(fnamew);
    // ソートフラグと, 引き渡し用適応度配列
    int flag, fitness[POPULATION];
    // 対戦・ソート用シード
    srand(seed1);
    // 適応度評価とファイルのソート
    // さらに適応度ファイルの読み書きも行う
    // 適応度評価方法はグローバル変数から参照 (マクロで決まる)
    // エラー
    if ((flag = sortSprmCompFileByFitness(EF_FUNC_SPRM, fnames, fitness)) < 0) return -1;
    // ソート済みであることを伝える
    if (flag == 1) {
        printf("%s is already sorted!\n", fnames);
    }
    // 選択・交叉・突然変異用シード
    srand(seed2);
    // ソート済み適応度を表示
    printDecimalArray(fitness, POPULATION);
    // 合計値を確認 (適応度評価法が判断できるかも?)
    printf("適応度合計: %d\n", sumInt(fitness, POPULATION));
    // 今世代と次世代の個体配列を宣言
    Sprm current[POPULATION], next[POPULATION];
    // ソート済み個体配列を読み込む
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
    /*
    乱数に影響が出ないように次世代を作ったら勝率計算 (たまーに)
    ループ関数じゃなくてこっちでやった方がロードの手間は少ない
    適応度に対ランダム勝ち点を使う場合はわざわざ確認しなくてもよさそう
    if (!(gene_num % 20)) {
        kugiri(100);
        int game_num = 500;
        printf("the number of games: %d x 2\n", game_num);
        printf("the strongest:\n");
        calcWinRateSprmVSRandTotal(current[0], game_num);
        printf("the weakest:\n");
        calcWinRateSprmVSRandTotal(current[POPULATION - 1], game_num);
    }
    */
    // ソート済みフラグは立てずに書き込み
    dumpSprmFileCompDirectExit(fnamew, next, 0);
    return 0;
}

// 圧縮版次世代作成関数をループさせる関数
// 引数は開始世代番号と, 終了世代番号に変更 (最終世代はファイル作成のみ)
int nGeneSprmCompLoop(scmSprmSorted scm, const char *format, int safety, int start, int stop) {
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
        if (nGeneSprmComp(scm, format, gene_num, s1, s2, safety) < 0)
            return -1;
        // get time
        time(t_arr + 1);
        printElapsedTime(t_arr[1] - t_arr[0]);
        kugiri(100);
    }
    return 0;
}

// ソートだけ, 再現性があること前提だが
void sortOnlySprmComp(scmSprmSorted scm, const char *format, int gene_num) {
    u_int s1;
    int flag, fitness[POPULATION];
    char fnames[FILENAME_MAX];
    // ループと同一シードを使う
    printf("seed1: %d\n", s1 = SEED + gene_num + 1);
    // ソート対象ファイル名を作成
    snprintf(fnames, FILENAME_MAX, format, gene_num);
    srand(s1);
    // 適応度評価とファイルのソート
    // ソート済ならその旨を伝える
    if ((flag  = sortSprmCompFileByFitness(EF_FUNC_SPRM, fnames, fitness)) == 1) {
        printf("%s is already sorted!\n", fnames);
    }
    if (flag < 0) return;
    // ソートに成功, もしくはソート済なら適応度を表示
    printDecimalArray(fitness, POPULATION);
}

// フォーマットの世代数を獲得 (圧縮ファイル限定)
int getGeneNumComp(const char *format, int loc_pop) {
    int i, flag;
    // ロードしたパラメータの仮置き場
    Sprm pra[loc_pop];
    i = 0;
    while (1) {
        // エラーならその時点で抜ける
        if ((flag = loadSprmFileComp(format, i, pra, loc_pop)) < 0) {
            // エラーのひとつ前を返す (最終世代がソート済で途切れている可能性もある)
            // もし 0 世代からエラーなら -1 が返ってエラーとして扱えそう
            i--;
            if (i >= 0) {
                printf("%d 世代目までソート済ですが, %d 世代目が存在しません.\n", i, i + 1);
            }
            break;
        }
        // 未ソートの場合
        else if (flag == 0) {
            printf("%d 世代目が未ソートです.\n", i);
            break;
        }
        // ソート済みなら何もしない
        i++;
    }
    return i;
}

// Sprm の条件を与え, ファイル名を作成し, どの存在するシードを表示させたい
// 適応度ID, 圧縮判定, 個体数, エリート数, 交叉ID, 突変ID, 突変率, その他
int searchSeedSprm(int eff_id, int is_comp, int loc_pop, int loc_eln, int crs_id, int mut_id, double loc_mr, double lncr) {
    char format[FILENAME_MAX], fname[FILENAME_MAX];
    FILE *fp;
    // 適当にバッファを作る
    int c, seed_arr[BUF_LEN];
    c = 0;
    printf("公比の対数 %+6.3f で検索\n", lncr);
    // 3桁の値を全てチェック
    for (int s = 0; s < 1000; s++) {
        makeSprmFileFormatAuto(format, FILENAME_MAX, eff_id, is_comp, loc_pop, loc_eln, 3, crs_id, mut_id, loc_mr, s, lncr, ALPHA_BLX, ZAKO_FIRST);
        // 初期世代ファイル名でチェック
        snprintf(fname, FILENAME_MAX, format, 0);
        // 読み込み専用で開けたら, シードを表示
        if ((fp = fopen(fname, "rb")) != NULL) {
            fclose(fp);
            seed_arr[c++] = s;
        }
    }
    if (c) {
        printf("初期世代があるシード値: ");
        printDecimalArray(seed_arr, c);
    } else {
        puts("シードが見つかりません");
    }
    return 0;
}