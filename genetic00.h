#ifndef GENETIC00_h
#define GENETIC00_h

// 最初に思いついたでかいパラメータ
// 次はいつ使うんだろうね
typedef struct param {
    float weight1[32][64];
    float weight2[16][32];
    float weight3[8][16];
    float weight4[4][8];
    float weight5[2][4];
    float weight6[2];
    float bias1[32];
    float bias2[16];
    float bias3[8];
    float bias4[4];
    float bias5[2];
} Param;

// パラメータチェック
int checkParam(Param pr);

#endif