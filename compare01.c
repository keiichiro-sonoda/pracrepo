/*
異なる公比で進化させた個体どうしを対戦させてみたい
*/
#include "genetic02.h"

int main(void) {
    Sprm test;
    char format[FILENAME_MAX];
    makeSprmFileFormatAuto(format, FILENAME_MAX, 0b010, 1, 50, 0, 3, 5, 0, .01, 555, 0.001, 0);
    puts(format);
    test = loadRepSprmComp(format, 100, 50);
    showSprmOneLine(test);
    return 0;
}