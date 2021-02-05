// this is a source file for debugging
// デバッグ用ファイル
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort01.h"
#include "genetic02.h"

typedef char* char_p;

// 選択のデモ用関数
void selDemo(double cr, int loc_pop, int n) {
    puts("選択のデモ");
    int parents[2];
    double geo_prog[loc_pop];
    geoProg(geo_prog, loc_pop, 1., cr);
    for (int i = 0; i < n; i++) {
        rouletteDoubleMltDep(geo_prog, loc_pop, parents, 2);
        printDecimalArray(parents, 2);
    }
}

// 色を決める
void detColor(float weight) {
    u_char rep_colors[3][3] = {{0xe0, 0x66, 0x66}, {0xff, 0xf2, 0xcc}, {0x6f, 0xa8, 0xdc}}, color[3], i;
    if (weight >= 0) {
        for (i = 0; i < 3; i++) {
            color[i] = rep_colors[1][i] + (rep_colors[0][i] - rep_colors[1][i]) * weight * 2.f;
        }
    } else {
        for (i = 0; i < 3; i++) {
            color[i] = rep_colors[1][i] + (rep_colors[1][i] - rep_colors[2][i]) * weight * 2.f;
        }
    }
    printHexArrayDig(color, 3, 2);
    //printDecimalArrayDig(color, 3, 3);
}

int main(void) {
    setCORR_TABLE();
    initBoard();
    srand(time(NULL));
    char format[FILENAME_MAX];
    makeSprmFileFormatAuto(format, FILENAME_MAX, 0b010, 1, 50, 0, 3, 5, 0, .01, 555, -0.025, 0);
    changeDirPath(format, FILENAME_MAX, DIR00);
    puts(format);
    //checkSprmFileComp(format, 100, 50);
    Sprm pr = loadRepSprmComp(format, 100, 50, 0);
    showSprmOneLine(pr);
    puts("デバッグ中");
    return 0;
}