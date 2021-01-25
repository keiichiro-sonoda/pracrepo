/*
異なる公比で進化させた個体どうしを対戦させてみたい
*/
#include "genetic02.h"

int main(void) {
    Sprm test;
    char fname[FILENAME_MAX];
    makeSprmFileFormatAuto(fname, FILENAME_MAX, 0b010, 1, 50, 0, 3, 5, 0, 0.01f, 555, 0.001);
    puts(fname);
    return 0;
}