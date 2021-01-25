/*
異なる公比で進化させた個体どうしを対戦させてみたい
*/
#include "genetic02.h"

// 比較対象のシード値
#define COMPARE_SEED 555

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
    count = 0;
    for (i = start_th, count = 0; i <= stop_th; i += step_th, count++) {
        loc_cr_ln = (double)i / 1000;
        makeSprmFileFormatAuto(format, FILENAME_MAX, 0b010, 1, 50, 0, 3, 5, 0, .01, COMPARE_SEED, loc_cr_ln, 0);
        puts(format);
        rep_pra[count] = loadRepSprmComp(format, gene_num, loc_pop);
        showSprmOneLine(rep_pra[count]);
        putchar(10);
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