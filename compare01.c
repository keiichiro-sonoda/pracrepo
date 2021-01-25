/*
異なる公比で進化させた個体どうしを対戦させてみたい
*/
#include "genetic02.h"

// 公比どうしリーグ戦?
// 公比の自然対数は1000倍した整数値で与える
// まずは白黒1回ずつ, 手は最大評価値確定でいこう
void vsOtherCommonRatio(int start_th, int stop_th, int step_th, int gene_num) {
    int i;
    double loc_cr_ln;
    for (i =  start_th; i <= stop_th; i += step_th) {
        loc_cr_ln = (double)i / 1000;
        printFloat(loc_cr_ln);
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