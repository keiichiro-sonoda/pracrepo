/*
異なる公比で進化させた個体どうしを対戦させてみたい
*/
#include "genetic02.h"

// 比較対象のシード値
#define COMPARE_SEED 555

// 指し手ルーレットで対戦する際のシード値
#define VS_SEED 125

// 配列Aの各要素に配列Bの各要素を足す
#define addArray(A, B, n) for (int _ = 0; _ < (n); _++) (A)[_] += (B)[_]

// 盤面を返り値とする関数の型 (引数は自由?)
typedef Board (*decNxtSprmC)();

// Sprm で計算された最善手を選ぶ
Board getBestBoardSprm(const Board *next_boards, int n, const Sprm *prp, int color) {
    float mx_point = -FLT_MAX;
    float t_point;
    Board best_board;
    int inv;
    // 1 -> 1, 2 -> -1
    inv = -2 * color + 3;
    for (int i = 0; i < n; i++) {
        // sign inversion!!
        t_point = evaluationSimple(next_boards[i], *prp) * inv;
        if (mx_point < t_point) {
            // update
            mx_point = t_point;
            best_board = next_boards[i];
        }
    }
    return best_board;
}

// 手をルーレット選択で決める
// n: the number of next boards
// use simple parameter
// decide next board by roulette
Board getBoardSprmRoulette(const Board *next_boards, int n, const Sprm *prp, int color) {
    double s, exp_points[n];
    int choosed;
    // 黒なら10, 白なら-10
    int mag;
    mag = (-2 * color + 3) * 10;
    for (int i = 0; i < n; i++) {
        // evaluate all next boards
        // and calculate the power of e (to make numbers positive)
        exp_points[i] = exp(evaluationSimple(next_boards[i], *prp) * mag);
    }
    s = sumDouble(exp_points, n);
    choosed = rouletteDouble(exp_points, n, s);
    return next_boards[choosed];
}

// 非正規化対戦
int oneToOneSprm(decNxtSprmC dnfuncc, const Sprm *bpp, const Sprm *wpp) {
    Board main_board, nba[NEXT_MAX];
    int turn, pass, n, dif, kc[3], cpa[NEXT_MAX];
    Sprm pra[2];
    pra[0] = *bpp;
    pra[1] = *wpp;
    pass = 0;
    // set turn
    turn = 0b01;
    // set initial board
    main_board = START;
    while (1) {
        //showBoard(main_board);
        // calculate next
        // can't put a piece anywhere
        if ((n = canPutPP(main_board, turn, cpa, nba, kc)) == 0) {
            // can't do anything one another
            if (pass) break;
            // pass
            turn ^= 0b11;
            pass = 1;
            continue;
        }
        pass = 0;
        // determine a next board
        main_board = dnfuncc(nba, n, pra + turn - 1, turn);
        // switch turn
        turn ^= 0b11;
    }
    // difference between black and white
    if ((dif = kc[1] - kc[2]) > 0) {
        return 0b01;
    } else if (dif < 0) {
        return 0b10;
    }
    return 0b00;
}

// 個体数を指定できるリーグ戦関数
// win: +2, draw: +1, lose: 0
// give a function pointer to decide the next board
void leagueMatchSprmFF(decNxtSprmC dnfuncc, const Sprm *pra, int *result, int loc_pop) {
    int j;
    zeros(result, loc_pop);
    for (int i = 0; i < loc_pop; i++) { // 黒側の添字
        for (j = 0; j < loc_pop; j++) { // 白側の添字
            if (i == j) continue;
            // 非正規化対戦
            switch(oneToOneSprm(dnfuncc, pra + i, pra + j)) {
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

// 複数回対戦するリーグ戦関数 (指し手はルーレット選択で固定)
// 対戦数, シード値も与える
// win: +2, draw: +1, lose: 0
void leagueMatchSprmMlt(const Sprm *pra, int *result, int loc_pop, int n, unsigned loc_seed) {
    int tmp_rslt[loc_pop];
    zeros(result, loc_pop);
    // シード設定
    srand(loc_seed);
    for (int k = 0; k < n; k++) { // 指定した数だけ
        leagueMatchSprmFF(getBoardSprmRoulette, pra, tmp_rslt, loc_pop);
        addArray(result, tmp_rslt, loc_pop);
    }
}

// 公比どうしリーグ戦?
// 公比の自然対数は1000倍した整数値で与える
// まずは白黒1回ずつ, 手は最大評価値確定でいこう
// 指し手決定関数は引数で与える
void vsOtherCommonRatio(int start_th, int stop_th, int step_th, int gene_num, decNxtSprmC dnfuncc) {
    int i, loc_pop, count;
    double loc_cr_ln;
    char format[FILENAME_MAX];
    for (i = start_th, loc_pop = 0; i <= stop_th; i += step_th, loc_pop++);
    printDecimal(loc_pop);
    // 代表者配列
    Sprm rep_pra[loc_pop];
    int result[loc_pop];
    count = 0;
    for (i = start_th, count = 0; i <= stop_th; i += step_th, count++) {
        loc_cr_ln = (double)i / 1000;
        makeSprmFileFormatAuto(format, FILENAME_MAX, 0b010, 1, 50, 0, 3, 5, 0, .01, COMPARE_SEED, loc_cr_ln, 0);
        //puts(format);
        rep_pra[count] = loadRepSprmComp(format, gene_num, loc_pop);
    }
    // リーグ戦
    leagueMatchSprmFF(dnfuncc, rep_pra, result, loc_pop);
    printDecimalArray(result, loc_pop);
    leagueMatchSprmMlt(rep_pra, result, loc_pop, 10, VS_SEED);
    printDecimalArray(result, loc_pop);
}

int main(void) {
    initBoard();
    setCORR_TABLE();
    Sprm test;
    char format[FILENAME_MAX];
    makeSprmFileFormatAuto(format, FILENAME_MAX, 0b010, 1, 50, 0, 3, 5, 0, .01, 555, -0.01, 0);
    puts(format);
    //puts(format);
    test = loadRepSprmComp(format, 100, 50);
    showSprmOneLine(test);
    decNxtSprmC dnfuncc;
    dnfuncc = getBestBoardSprm;
    //dnfuncc = getBoardSprmRoulette;
    srand(VS_SEED);
    //vsOtherCommonRatio(-2000, 2000, 100, 100, dnfuncc);
    // 公比1以下のみ
    vsOtherCommonRatio(-2000, 0, 100, 100, dnfuncc);
    return 0;
}