/*
異なる公比で進化させた個体どうしを対戦させてみたい
*/
#include "genetic02.h"

// 比較対象のシード値
#define COMPARE_SEED 555
// 指し手ルーレットで対戦する際のシード値
#define VS_SEED 123

// 個体数を指定できるリーグ戦関数
// win: +2, draw: +1, lose: 0
// give a function pointer to decide the next board
void leagueMatchSprmFF(decNxtSprm dnfunc, const Sprm *generation, int *result, int loc_pop) {
    int j;
    zeros(result, loc_pop);
    for (int i = 0; i < loc_pop; i++) { // 黒側の添字
        for (j = 0; j < loc_pop; j++) { // 白側の添字
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

// 公比どうしリーグ戦?
// 公比の自然対数は1000倍した整数値で与える
// まずは白黒1回ずつ, 手は最大評価値確定でいこう
void vsOtherCommonRatio(int start_th, int stop_th, int step_th, int gene_num) {
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
        puts(format);
        rep_pra[count] = loadRepSprmComp(format, gene_num, loc_pop);
    }
}

int main(void) {
    Sprm test;
    char format[FILENAME_MAX];
    makeSprmFileFormatAuto(format, FILENAME_MAX, 0b010, 1, 50, 0, 3, 5, 0, .01, 555, -0.01, 0);
    //puts(format);
    test = loadRepSprmComp(format, 100, 50);
    showSprmOneLine(test);
    vsOtherCommonRatio(-2000, 2000, 100, 100);
    return 0;
}