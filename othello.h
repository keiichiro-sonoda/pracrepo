#ifndef OTHELLO_H
#define OTHELLO_H

// maximum number of next boards
#define NEXT_MAX 32
#define MASU_NUM 64
// 1 million
#define MILLION 1000000
// 1 billion
#define BILLION 1000000000
// initial board information
#define START_H 0x0000000000000180L
#define START_L 0x0240000000000000L

// sample board information
#define SAMPLE1_A 0x0000000000000180L
#define SAMPLE1_B 0x0280008000000000L

// bin -> char
// black: (0b01, o), white: (0b10, x), empty: (0b00, -)
// can put sign: (0b11, !)
#define B2C "-ox!"

// macros
// for debug
#define kugiri() printf("--------------------------\n")
#define printString(s) printf("%s\n", s)
#define printDecimal(x) printf("%d\n", x)
#define printFloat(x) printf("%f\n", x)
#define printSize(x) printf("%ld\n", sizeof x)
#define arrayLength(A) sizeof(A) / sizeof(A[0])

// get the smaller number
#define getMin(a, b) ((a) < (b) ? (a) : (b))

// flip horizontal
// src: 0, 2, 4, ... , 62
#define mirrorLRAd(src) (src) / 16 * 16 + 14 - (src) % 16

// 64bit
typedef unsigned long int int8B;

// othello board
// 128 bits
typedef struct {
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
// print an array of floating point number
void printFloatArray(const float *A, int n);
// print an arrya of hexadecimal numbers
int showHexArray(int *ia, int ia_len);
// neighbor?
int ifNeighbor(int src, int dst);
// check if the element is in the array
int inArray(int *ar, int ar_len, int el);
// get index of el?
int getIndex(const int *ar, int ar_len, int el);
// get maximum value of array
int getMax(int *ar, int ar_len);
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

// all zero
void zeros(int *A, int n);
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
int rotL90DegAd(int src);
// rotate the board 90 degrees to the left
Board rotL90DegBoard(Board b1);
Board mirrorHLBoard(Board b1);
// get the smaller board
Board minBoard(Board b1, Board b2);
// normalize a board
Board normalBoard(Board b1);
// initial configure
void initBoard(void);
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

#endif