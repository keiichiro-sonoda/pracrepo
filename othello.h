#ifndef OTHELLO_H
#define OTHELLO_H

// マクロで使うライブラリ
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// maximum number of next boards
#define NEXT_MAX 32
#define MASU_NUM 64
// 1 million
#define MILLION 1000000
// 1 billion
#define BILLION 1000000000
// initial board information
#define START_A 0x0000000000000180L
#define START_B 0x0240000000000000L

// sample board information
#define SAMPLE1_A 0x0000000000000180L
#define SAMPLE1_B 0x0280008000000000L

// bin -> char
// black: (0b01, o), white: (0b10, x), empty: (0b00, -)
// can put sign: (0b11, !)
#define B2C "-ox!"

// macros
// for debug
#define kugiri(length) for (int _ = 0; _ < length; _++) putchar('-'); putchar('\n')
#define printString(s) printf("%s\n", s)
#define printDecimal(x) printf("%d\n", x)
#define printFloat(x) printf("%f\n", x)
#define printSize(x) printf("%ld\n", sizeof x)
#define arrayLength(A) sizeof(A) / sizeof(A[0])
// char型の整数表現を確認する (多分32bit以内ならchar以外も可)
#define printCharDecimal(x) printf("%d\n", (int)(x))

// get the smaller number
#define getMin(a, b) ((a) < (b) ? (a) : (b))

// get the larger number
#define getMax(a, b) ((a) > (b) ? (a) : (b))

// 配列を定数で初期化する
#define initArrayConst(A, n, c) for (int _ = 0; _ < (n); _++) (A)[_] = (c)

// set all elements to ones
#define ones(A, n) initArrayConst(A, n, 1)

// 全て0で初期化
#define zeros(A, n) initArrayConst(A, n, 0)

// 配列コピー (型に依存しないって便利ね)
// copy an array
#define copyArray(src, dst, n) for (int _ = 0; _ < (n); _++) (dst)[_] = (src)[_]

// subract constant value from all elements
#define subConst(A, n, c) for (int _ = 0; _ < (n); _++) (A)[_] -= (c)

// flip horizontal
// src: 0, 2, 4, ... , 126
#define mirrorLRAd(src) ((src) / 16 * 16 + 14 - (src) % 16)

// rotate 90 degrees to the left
// src: 0, 2, 4, ... , 126
#define rotL90DegAd(src) ((14 - (src) % 16) * 8 + (src) / 16 * 2)

// calculate the square
#define square(x) ((x) * (x))

// 二乗距離を計算
#define sqDist(a, b) square((a) - (b))

// ベクトル A と B の各要素の二乗距離を計算し D に代入
// D の総和の平方根が A と B の距離になる
#define sqDistArray(A, B, D, n) for (int _ = 0; _ < (n); _++) (D)[_] = sqDist((A)[_], (B)[_])

// 0以上x未満の整数乱数を返すマクロ
#define randInt(x) (rand() % (x))

// return an integer random number from min to max
// positive range only
#define randIntRange(min, max) (randInt((max) - (min) + 1) + (min))

// 0.0 以上 1.0 以下の float 型乱数を返す
#define randFloat() ((float)rand() / RAND_MAX)

// 0 か 1 を返すだけのマクロ
#define randBit() (rand() & 1)

// 32bitバイナリ表示するマクロ(ハイフン付き)
// 波括弧を使わない if 節で使う場面を想定
#define printBin32(x) do {printf("0b"); for (char _ = 31; _ >= 0; _--) {printf("%d", (x >> _) & 1); if (!(_ % 4) && _) putchar('-');} putchar(10);} while (0)

// 初期盤面の設定
// initial configure
#define initBoard() do {START.board[1] = START_A; START.board[0] = START_B; SAMPLE1.board[1] = SAMPLE1_A; SAMPLE1.board[0] = SAMPLE1_B;} while (0)

// return とかマクロで書いていいのかな
#define warnOverwritingExit(fname) if (warnOverwriting(fname) < 0) return -1  

// 当然ロード版も作るよね
#define loadFileDirectExit(fname, xp, x_size) do {\
    FILE *_fp;\
    if ((_fp = fopen((fname), "rb")) == NULL) {\
        printf("\a%s can't be opened.\n", (fname));\
        return -1;\
    }\
    fread((xp), (x_size), 1, _fp);\
    fclose(_fp);\
} while (0)

// マクロで型に依存せずにバイナリファイル作れるんじゃね?
// ファイル開けなかったら, マクロ呼び出し元で戻る
// 注意すべきことは, 呼び出し元が数値を返す関数だってこと
// charでもfloatでも大丈夫なのかな?
#define dumpFileDirectExit(fname, xp, x_size) do {\
    FILE *_fp;\
    if ((_fp = fopen((fname), "wb")) == NULL) {\
        printf("\a%s can't be opened.\n", (fname));\
        return -1;\
    }\
    fwrite((xp), (x_size), 1, _fp);\
    fclose(_fp);\
} while (0)

// short型で保存されている適応度を読み込む
// othelloに移動したはいいが POPULATION が無いので引数で定義
// マクロ中毒
#define loadFitnessShortDirectExit(fname, fitness, n) do {\
    short fitness_short[n];\
    loadFileDirectExit((fname), fitness_short, sizeof fitness_short);\
    copyArray(fitness_short, (fitness), (n));\
} while (0)

// 適応度書き込み
// 上書き要注意
#define dumpFitnessShortDirectExit(fname, fitness, n) do {\
    short fitness_short[n];\
    copyArray((fitness), fitness_short, (n));\
    dumpFileDirectExit((fname), fitness_short, sizeof fitness_short);\
} while (0);

// ソート済み適応度配列を格納するファイル名を作る関数型マクロ
// 適応度評価対象のファイル名から作成
#define makeFitnessFileNameDirectExit(dst, dst_size, fnameo) do {\
    /* .binを省いた文字列を格納できる限界の長さ */\
    int _len = strlen(fnameo) - 3;\
    if (_len + 11 >= (dst_size)) {printf("\afile name over\n"); return -1;}\
    const char fitness_format[] = "%s_fitness.bin";\
    char fnameo_part[_len];\
    snprintf(fnameo_part, _len, "%s", (fnameo));\
    snprintf((dst), dst_size, fitness_format, fnameo_part);\
} while (0)

// 64bit
typedef unsigned long int int8B;

// othello board
// 128 bits
typedef struct board{
    // low bits, high bits
    int8B board[2];
} Board;

// intial board
extern Board START;
// sample board
extern Board SAMPLE1;

// 8 directions
extern const int DIRECTION[8];

// get a piece at a certain address
int getKoma(Board b, int ad);
// put a piece at a certain address
void putKoma(Board *bp, int ad, int koma);
// empty the board
void emptyBoard(Board *bp);
// all zero
Board createEmptyBoard(void);
// show a board
int showBoard(Board b);
// show board in 32 hexadecimal digits
int showBoardHex(Board b);
// print an array of decimal numbers
void printDecimalArray(const int *A, int n);

// show the first three and the last one
void printDecimalArrayPart(const int *A, int n);

// show [a, b)
void printDecimalArrayRange(const int *A, int a, int b);

// print an array of floating point number
void printFloatArray(const float *A, int n);

// show the first three and the last one
void printFloatArrayPart(const float *A, int n);

// print an arrya of hexadecimal numbers
int showHexArray(int *ia, int ia_len);
// neighbor?
int ifNeighbor(int src, int dst);
// check if the element is in the array
int inArray(int *ar, int ar_len, int el);
// get index of el?
int getIndex(const int *ar, int ar_len, int el);

// get maximum value of int array
int getMaxIntArray(int *A, int n);

// reverse a piece at a certain address
// give a Board pointer to rewrite it
void reOneAd(Board *bp, int ad);
// reverse pieces in some addresses?
void reRange(Board *bp, int *ads, int length);
// find addresses to put black
// and calculate nextboard
int canPutBlackPlus(Board b, int *cpb, Board *nbs);
// return next board (put black)
// if invalid action, return same board
Board nextBoardBlack(Board b, int te);
// return next board (put white)
Board nextBoardWhite(Board b, int te);
// find addresses to put white (black base)
// and calculate nextboard
int canPutWhitePlus(Board b, int *cpw, Board *nbs);
// show some boards
void showBoardArray(const Board *ba, int ba_len);
// swap white and black
Board swapBoard(Board b);
// get action from standard input
int getActStdin(void);

int getValidActStdin(int *can_put, int length);

int ad2coo(int ad, char *dst);

int showCoordinates(const int *can_put, int length);

int showCanPut(Board b, const int *can_put, int next_count);

int showCanPutPlus(Board b, int color, int *can_put, Board *next_boards);

// all zero (float)
void zerosFloat(float *A, int n);

// all zero 2D array
// int iaa[l1][l2]
int zeros2D(int *iaa[], int l1, int l2);

// 0, 1, 2, ..., n - 1
void indices(int *A, int n);

// 5 arguments
// count the number of pieces
int canPutPP(Board b, int color, int *can_put, Board *next_boards, int *koma_count);
int showCanPutPP(Board b, int color, int *can_put, Board *next_boards);
// nega max
int negaMax(Board b, int color, int depth, int pass);
int wrapNegaMax(Board b, int color);
// alpha beta
int negaMaxAB(Board b, int color, int depth, int pass, int alpha, int beta);
int wrapNegaMaxAB(Board b, int color, int height);
int play(void);
// rotate the board 90 degrees to the left
Board rotL90DegBoard(Board b1);
Board mirrorHLBoard(Board b1);
// get the smaller board
Board minBoard(Board b1, Board b2);
// normalize a board
Board normalBoard(Board b1);

void normalizeBoard(Board *bp);
// swap and normalize a board
void swapNormalizeBoard(Board *bp);
// 0 or 1
int sameBoard(Board b1, Board b2);
// check if it's a known board
int knownBoard(const Board *ba, int n, Board b);
// give a normalized board
// next turn is always black
int nextBoardNormal(Board b, Board *next_boards);
// count the number of pieces
int nextBoardNormal2(Board b, Board *next_boards, int *koma_count);
// black: +1, empty: 0, white: -1
void board2arraySymmetry(Board src, int *dst);

// warning before overwriting
int warnOverwriting(const char *fname);

#endif