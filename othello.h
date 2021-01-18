#ifndef OTHELLO_H
#define OTHELLO_H

// マクロで使うライブラリ
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// ??
#include <sys/stat.h>
#include <sys/types.h>

// maximum number of next boards
#define NEXT_MAX 32
#define MASU_NUM 64
// 1 million
#define MILLION 1000000
// 1 billion
#define BILLION 1000000000
// initial board information
#define START_A 0x0000000000000240ul
#define START_B 0x0180000000000000ul

// sample board information
#define SAMPLE1_A 0x0000000001000140ul
#define SAMPLE1_B 0x0180000000000000ul

// bin -> char
// black: (0b01, o), white: (0b10, x), empty: (0b00, -)
// can put sign: (0b11, !)
#define B2C "-ox!"

// macros
// for debug
#define kugiri(n) do {for (int _ = 0; _ < (n); _++) putchar('-'); putchar(10);} while (0)
#define printString(s) printf("%s\n", s)
// キャストで汎用性を高めた? 符号付き32ビット限定
#define printDecimal(x) printf("%d\n", (int)(x))
#define printUDecimal(x) printf("%u\n", (unsigned)(x))
// 16進法表示 (あえて符号なしにする意味はあったか?)
// 関数ポインタの表示に役立つ?
#define printHex64(x) printf("%lx\n", (u_long)x)
#define printFloat(x) printf("%f\n", x)
#define printFloatExp(x) printf("%+.20e\n", x) // 浮動小数点数を指数表示する
#define printSize(x) printf("%ld\n", sizeof x)
#define arrayLength(A) (sizeof(A) / sizeof(A[0]))
// 経過時間を表示 (秒数を時間, 分, 秒に変換)
// 大きすぎる値や, 負の値は考慮しない
// 一時変数で型を柔軟にする? さらに同じ式を何度も繰り返すのも回避?
#define printElapsedTime(sec) do {long _tmp = (sec); printf("%02ld 時間 %02ld 分 %02ld 秒\n", _tmp / 3600, _tmp % 3600 / 60, _tmp % 60);} while (0)
// 桁数も指定できる10進配列表示関数を作りたい
#define printDecimalArrayDig(A, n, d) do {\
    char _ff1[BUF_LEN], _ff2[BUF_LEN];\
    snprintf(_ff1, BUF_LEN, "{%%%dd", d);\
    snprintf(_ff2, BUF_LEN, ", %%%dd", d);\
    printf(_ff1, (int)(A)[0]);\
    for (int _ = 1; _ < (n); _++) printf(_ff2, (int)(A)[_]);\
    printf("}\n");\
} while (0)

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

// 初項 ft, 項比 cr の等比数列を作成
// 初項は A[0]
// 1つ前の要素に公比を掛けてもいいが, 初項の処理が面倒なので一般項を用いる
#define geoProg(A, n, ft, cr) for (int _ = 0; _ < (n); _++) (A)[_] = (ft) * pow((cr), _)

// subract constant value from all elements
#define subConst(A, n, c) for (int _ = 0; _ < (n); _++) (A)[_] -= (c)

// 平方根の関数を作成
#define sqrtArray(X, Y, n) for (int _ = 0; _ < (n); _++) (Y)[_] = sqrt((X)[_])

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

// 0.0 以上 1.0 以下の double 型一様乱数を返す
#define randDouble() ((double)rand() / RAND_MAX)

// min 以上 max 以下の double 型一様乱数を返す
#define randDoubleRange(min, max) (randDouble() * ((max) - (min)) + (min))

// 0 か 1 を返すだけのマクロ
#define randBit() (rand() & 1)

// type型の変数xとyの値を交換
#define swap(type, x, y) do {type _t; _t = (x); (x) = (y); (y) = _t;} while (0)

// x が min 未満なら min を, max より大きければ max を, 間にあれば x を返す
// 先に min と比較
#define clamp(x, min, max) (((x) < (min) ? (min) : ((max) < (x) ? (max) : (x))))

// 32bitバイナリ表示するマクロ(ハイフン付き)
// 波括弧を使わない if 節で使う場面を想定
#define printBin32(x) do {printf("0b"); for (char _ = 31; _ >= 0; _--) {printf("%d", (x >> _) & 1); if (!(_ % 4) && _) putchar('-');} putchar(10);} while (0)

// マクロに変更
// print an array of floating point number
#define printFloatArray(A, n) do {printf("{%6.3f", (A)[0]); for (int _ = 1; _ < (n); _++) printf(", %6.3f", (A)[_]); puts("}");} while (0)

// 浮動小数点数の配列の要素を指数表示する
// 関数化すると float と double の差で警告が出るのでマクロを使う
#define printFloatArrayExp(A, n) do {printf("{%.3e", (A)[0]); for (int _ = 1; _ < (n); _++) printf(", %.3e", (A)[_]); puts("}");} while (0)

// 指定したアドレスのコマを取得
// b は Board型を想定, ad は整数ならなんでもよさそう
// ad >> 6 が 0 なら下半分, 1 なら上半分
// get a piece at a certain address
#define getKoma(b, ad) (((b).board[(ad) >> 6] >> ((ad) & 0x3f)) & 0b11)

// コマを置く (対象は空マスを想定)
// 関数だったころの名残で盤面のポインタを渡す
// put a piece at a certain address
#define putKoma(bp, ad, koma) ((bp)->board[(ad) >> 6] |= (int8B)(koma) << ((ad) & 0x3f))

// 指定した座標のコマを反転 (対象は空マス以外を想定)
// reverse a piece at a certain address
// give a Board pointer to rewrite it
#define reOneAd(bp, ad) ((bp)->board[(ad) >> 6] ^= (int8B)0b11 << ((ad) & 0x3f))

// ads には反転対象のアドレス配列を与える
// reverse pieces in some addresses?
#define reRange(bp, ads, n) for (int _ = 0; _ < (n); _++) reOneAd(bp, ads[_])

// srcから見てdstが繰り下がっていないかどうかをチェック
// isNeighbor で呼び出されることを想定
// src が最右列でないかつ, dst が最左列でないなら 1 を返す
#define notCarryDown(src, dst) ((src) % 16 || (dst) % 16 != 14)

// 与えられた座標同士が隣り合う座標なのか確認
// 繰り上 (下) がりしているのか, 範囲外なのか
// src は範囲内であること前提とする
#define isNeighbor(src, dst) (0 <= (dst) && (dst) <= 126 && notCarryDown(src, dst) && notCarryDown(dst, src))

// 初期盤面の設定
// initial configure
#define initBoard() do {START.board[1] = START_A; START.board[0] = START_B; SAMPLE1.board[1] = SAMPLE1_A; SAMPLE1.board[0] = SAMPLE1_B;} while (0)

// 盤面を空にする
// 宣言文を使わなければ括弧が便利かも
// 不要だが返り値は 0
// empty the board
#define emptyBoard(bp) ((bp)->board[0] = 0, (bp)->board[1] = 0)

// 連結先のサイズを指定して安全? に文字列連結
// 連結後の文字列の長さがサイズ未満なら連結
// サイズぴったりだとナル文字が置けない?
#define strcatSize(dst, src, dst_size) if (strlen(dst) + strlen(src) < (dst_size)) strcat(dst, src)

// -Exit() というマクロは整数を返す関数内で使うことだけ想定
// 警告文は改行無しで別途表示するのがよい
// 標準入力で y と Enter だけ押されたら, 何もしない
// それ以外は勝手に -1 で抜ける
#define kakuninExit() do {\
    printf(" (y\\n): ");\
    char c;\
    if ((c = getchar()) != 121) {\
        if (c != 10) while (getchar() != 10);\
        printf("terminated\n");\
        return -1;\
    }\
    if (getchar() != 10) {\
        while (getchar() != 10);\
        printf("terminated\n");\
        return -1;\
    }\
} while (0)

// 上書き警告関数をマクロ化
// 存在したら kakuninExit () を使用
#define warnOverwritingExit(fname) do {\
    FILE *fp;\
    if ((fp = fopen(fname, "rb")) != NULL) {\
        fclose(fp);\
        printf("\a\"%s\" exists. Do you overwrite it?", fname);\
        kakuninExit();\
    }\
} while (0)

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

// 型に依存せずにバイナリファイルを作成??
// ファイル開けなかったら, マクロ呼び出し元を -1 で抜ける
// 呼び出し元は数値を返す関数前提
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
// fitness は short 型配列でなくてもよい
// othelloに移動したはいいが POPULATION が無いので引数で定義
// Python で読み込むことを考えると, 結果的に配列長を指定できるのは便利
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

// 環境によって適宜変更 (64bitの符号なし整数)
// 64bit の値が使えない環境は動作不可
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

// show the first three and the last one
void printFloatArrayPart(const float *A, int n);

// print an arrya of hexadecimal numbers
int showHexArray(int *ia, int ia_len);

// check if the element is in the array
int inArray(int *ar, int ar_len, int el);
// get index of el?
int getIndex(const int *ar, int ar_len, int el);

// get maximum value of int array
// 配列, 配列長
int getMaxIntArray(const int*, int);

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

// CUI 対戦
int play(void);

// 起こりうるランダムな盤面を返す関数
Board getRandPossibleBoard(void);

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

// 要素が 0, 1, 2 の配列をBoard型に変換
void array2board(const int *src, Board *dst);

// 標準入力で y を入力して許可してほしいけど, 関数を抜けたくない場合
int kakunin(void);

#endif