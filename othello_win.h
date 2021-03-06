#ifndef OTHELLO_WIN_H
#define OTHELLO_WIN_H

// maximum number of next boards
#define NEXT_MAX 32
// マスの数
#define MASU_NUM 64
// 1 million
#define MILLION 1000000
// 1 billion
#define BILLION 1000000000

#define START_H 0x0000000000000180L
#define START_L 0x0240000000000000L

// グローバル変数をマクロに変更
// bin -> char
// black: (0b01, o), white: (0b10, x), empty: (0b00, -)
// can put sign: (0b11, !)
#define B2C "-ox!"

// マクロ
#define kugiri(l) for (int _ = 0; _ < l; _++) putchar('-'); putchar(10)
#define printString(s) printf("%s\n", s)
#define printDecimal(x) printf("%d\n", x)
#define printFloat(x) printf("%f\n", x)
// mingw だとlong long unsigned int が sizeof の戻り値? らしい
#define printSize(x) printf("%I64u\n", sizeof x)
#define arrayLength(A) (sizeof(A) / sizeof((A)[0]))

// 配列を全て0にする
// 配列の型に依存しないから便利かもしれないが
// 予期せぬバグが起きるかもしれない
#define zeros(A, n) for (int _ = 0; _ < (n); _++) (A)[_] = 0

// 配列複製
#define copyArray(src, dst, n)  for (int _ = 0; _ < n; _++) (dst)[_] = (src)[_]

// 0以上n未満の整数の乱数を返す
// ubuntuのrandIntで範囲を指定したが, ここでは異なる
#define randInt(n) rand() % n

// 64bit
// mingw64 long long int
typedef unsigned long long int8B;

// othello board
// 128 bits
typedef struct board {
    // low bits, high bits
    int8B board[2];
} Board;

// 正規化された盤面と, 対応する手の配列を保存する構造体
// 手を指す前の盤面の情報は含まない
// 異なる手で等価な盤面になることは少ないと思うが
// 等価なマスの数だけ用意しておくことにする
// nは手の数
typedef struct boardact {
    Board nbd;
    int acts[8];
    int n;
} BoardAct;

// intial board
// 複数ファイルで共有するときは extern を付けるらしい
// const は諦めよう
extern Board START;

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

// 32桁の16進数で盤面を表示
// show board in 32 hexadecimal digits
void showBoardHex(Board b);

// 正規化された盤面と対応する指し手を確認する
void showBoardAct(BoardAct board_act);

// 盤面と指し手のペアの配列を確認
void showBoardActArray(const BoardAct *board_act_arr, int n);

// display int array (decimal)
// int型の配列を10進表記で表示
void printDecimalArray(const int *A, int n);

// float型の配列を表示
void printFloatArray(const float *A, int n);

// float型の配列の一部を表示
void printFloatArrayPart(const float *A, int n);

// display int array (hexadecimal)
int showHexArray(int *ia, int ia_len);

// neighbor?
int areNeighbors(int src, int dst);

// check if the element is in the array
int inArray(int *ar, int ar_len, int el);
// 添え字を取得(英語使うの面倒になった)
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
// 有効な手が入力されるまで繰り返す
int getValidActStdin(int *can_put, int length);
// アドレス(数値)を文字列に
int ad2coo(int ad, char *dst);
// 指せる手の座標を文字列で表示
int showCoordinates(const int *can_put, int length);
// 指せる手を ! で表示
int showCanPut(Board b, const int *can_put, int next_count);
// 指せる手を計算しつつ表示もする
int showCanPutPlus(Board b, int color, int *can_put, Board *next_boards);

// float型の配列の要素を全て0.0にする
void zerosFloat(float *A, int n);

// all zero 2D array
// int iaa[l1][l2]
// 確かボツになっていたが一応定義しておこう
int zeros2D(int *iaa[], int l1, int l2);
// 添え字がそのまま要素になる配列を作る
int indexes(int *ia, int ia_len);
// 5 arguments
// count the number of pieces
int canPutPP(Board b, int color, int *can_put, Board *next_boards, int *koma_count);
// 色指定して次の盤面を計算し, 表示まで行なう
int showCanPutPP(Board b, int color, int *can_put, Board *next_boards);
// nega max
int negaMax(Board b, int color, int depth, int pass);
// negaMax() のラッパー関数
int wrapNegaMax(Board b, int color);
// alpha beta
int negaMaxAB(Board b, int color, int depth, int pass, int alpha, int beta);
// negaMaxAB() のラッパー関数
int wrapNegaMaxAB(Board b, int color, int height);
// 標準入力で対戦したり, CPU同士で対戦したりする
int play(void);
// 左に90°回転した位置の座標に変換する
int rotL90DegAd(int src);
// rotate the board 90 degrees to the left
Board rotL90DegBoard(Board b1);
// 上下反転させた盤面を返す
Board mirrorHLBoard(Board b1);
// get the smaller board
Board minBoard(Board b1, Board b2);
// normalize a board
Board normalBoard(Board b1);
// initial configure
void initBoard(void);
// Board型ポインタを渡し, 正規化して書き換える
void normalizeBoard(Board *bp);
// swap and normalize a board
void swapNormalizeBoard(Board *bp);
// 同じ盤面かどうか判定
int sameBoard(Board b1, Board b2);
// check if it's a known board
int knownBoard(const Board *ba, int n, Board b);
// give a normalized board
// next turn is always black
int nextBoardNormal(Board b, Board *next_boards);
// count the number of pieces
int nextBoardNormal2(Board b, Board *next_boards, int *koma_count);

// 盤面と対応する指し手の配列を更新する
// 盤面は正規化したものを与える
// ここまでの盤面の数はindexに保存してあるものとし, 参照渡しで更新する
void updateBoardActArray(BoardAct *board_act_arr, Board n_board, int ad, int *index);

// 指し手の候補を作成する関数 (空マスベース探索)
// 与える盤面は正規化していないものとする (正規化しても問題ないが, 実用的でない?)
// 正規化した次の盤面の配列も作成する (評価用)
// 正規化して一致する手の場合, 最初に見つかったものだけ採用する
int canPutNet(Board b, int color, BoardAct *board_act_arr);

// Board 型を配列に変換
void board2array(Board src, int *dst);

// 配列をBoard型に変換
void array2board(int *src, Board *dst);

// black: +1, empty: 0, white: -1
void board2arraySymmetry(Board src, int *dst);
// まさか関数の定義だけでここまで長くなるとは

#endif