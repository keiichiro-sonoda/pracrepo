#ifndef GENETIC01_H
#define GENETIC01_H

// マクロで使うのが確定しているライブラリ
#include <float.h>

// この中でいくつかインクルード済み
#include "othello.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <math.h>

// MGG を使う際の個体数
#define MGG_NUM_PRM1L 100

// number of parameters per generation
// MGG の場合, 適応度計算時の個体数に用いる
#ifndef POPULATION
#define POPULATION 50 // 個体数
#endif

#ifndef ELITE_NUM
#define ELITE_NUM 6 // エリート数
#endif

#ifndef MUT_RATE
#define MUT_RATE .01 // 突然変異率
#endif

// ここで活性化関数を指定する
// specify the activation function here
#ifndef ACT_FUNC
#define ACT_FUNC ReLU
#endif

// BLX-α 交叉で使う α の値
// 噂によると 0.3 前後がおすすめらしい
#define ALPHA_BLX 0.3

// specify the seed value here
#ifndef SEED
#define SEED 123u
#endif

 // ランダム対戦の色ごとの対戦数
 // リーグ戦とのハイブリッドの場合は 1:1 くらいにしておこうかな
#define GAME_NUM_PRM1L 50

// バイアスの倍率
// 重みパターンと盤面との乗算結果は-32から32を取り得る (ただしその両端の値を取る確率は極めて低い. と思う)
// 最大は多分64にすればよいが、なんとなく半分くらいで十分だと思ってる
// 精度は同じchar型を使うが, 幅を広く持たせることにした
// このマクロが定義される以前のものは, これが1であると考えれば良い
#define BIAS_MAG 32

// レイヤ2の入力の長さ
// 変える予定は無いができる限り共通の数であることを主張したい
#ifndef PRM1L_L2_NUM
#define PRM1L_L2_NUM 8
#endif

// test file format
// common with genetic02
#define FNF_TEST "prm/test/test%03d.bin"

// MGG ファイルのテスト用フォーマット
#define FNF_MGG_TEST "prm/test/mgg.bin"

// L1 : the number of middle layer is 1
// R  : decide next board by roulette
// 050: population
// 06 : the number of elite
// 00 : roulette selection
// 00 : uniform crossover
// 00 : random mutation (or none)
// 001: mutation rate
// default is the step function
#define FNF_L1R05006000000001 "prm/l1r050_06_rlt_uni_rd001/l1r050_06_rlt_uni_rd001_g%03d.bin"

// R  : use ReLU as activation function
#define FNF_L1RR05006000000001 "prm/l1rr050_06_rlt_uni_rd001/l1rr050_06_rlt_uni_rd001_g%03d.bin"
// 突然変異率だけ変更
#define FNF_L1RR05006000000005 "prm/l1rr050_06_rlt_uni_rd005/l1rr050_06_rlt_uni_rd005_g%03d.bin"

// C  : ファイルを圧縮して保存 (compressの意味)
// M32: バイアスの倍率が32という意味
#define FNF_L1RRCM3205006000000001 "prm/l1rrcm32_050_06_rlt_uni_rd001/l1rrcm32_050_06_rlt_uni_rd001_g%03d.bin"

// 02: 一点交叉
#define FNF_L1RRCM3205006000200001 "prm/l1rrcm32_050_06_rlt_1p_rd001/l1rrcm32_050_06_rlt_1p_rd001_g%03d.bin"

// 03: 二点交叉
#define FNF_L1RRCM3205006000300001 "prm/l1rrcm32_050_06_rlt_2p_rd001/l1rrcm32_050_06_rlt_2p_rd001_g%03d.bin"

// 05: ブロックごと一様交叉 (3と4は多点交叉等の予約)
#define FNF_L1RRCM3205006000500001 "prm/l1rrcm32_050_06_rlt_ub_rd001/l1rrcm32_050_06_rlt_ub_rd001_g%03d.bin"

// don't use activation function
#define FNF_NON_AF "prm/l1r050_06_rlt_uni_rd001/non_act_func/l1r050_06_rlt_uni_rd001_g%03d.bin"

// return an floating point random number from -0.5 to 0.5
#define randWeight() (randFloat() - .5f)

// 圧縮された重みで使う符号付き文字型の乱数を作成 (-127から127)
#define randWeightChar() ((signed char)(rand() % 255 - 127))

// 重みをchar型に圧縮
// -0.5から0.5を-127から127へ (範囲外は考慮していない)
// char型も一応符号の概念があるっぽい (環境依存)
#define weight2char(w) ((signed char)((w) * 254))

// 圧縮された文字を小数に戻す
#define char2weight(c) ((float)(c) / 254)

// 圧縮対応の float 型乱数を作成
// 一旦文字型乱数を作成した後, 小数に変換
#define randWeightComp() char2weight(randWeightChar()) 

// create an array of random weights
#define randWeightArray(A, n) for (int _ = 0; _ < (n); _++) (A)[_] = randWeight()

// 圧縮されたchar型重みの乱数の配列を作成
// 初期世代ファイルに使う予定
#define randWeightCharArray(A, n) for (int _ = 0; _ < (n); _++) (A)[_] = randWeightChar()

// choose randomly either a or b
#define alternative(a, b) (randBit() ? (a) : (b))

// uniform crossover (array)
#define uniCrossArray(M, F, C, n) for (int _ = 0; _ < (n); _++) (C)[_] = alternative((M)[_], (F)[_])

// ここでマクロ MUT_RATE を使う
// MUT_RATE chance to replace with random value
#define randMutArray(C, n) for (int _ = 0; _ < (n); _++) if (randDouble() < MUT_RATE) (C)[_] = randWeight()

// 圧縮対応のランダム突然変異 (配列)
#define randMutArrayComp(C, n) for (int _ = 0; _ < (n); _++) if (randDouble() < MUT_RATE) (C)[_] = randWeightComp()

// activation functions
// sigmoid function (return float)
#define sigmoidFloat(x) (1.0f / (1 + expf(-10 * (x))))

// step function
#define step(x) ((x) > 0)

// rectified linear unit (ramp function)
#define ReLU(x) getMax(0, x);

// 圧縮されたchar型配列をfloat型配列に変換
#define char2weightArray(src, dst, n) for (int _ = 0; _ < (n); _++) (dst)[_] = char2weight((src)[_])
// float型配列をchar型配列に圧縮
#define weight2charArray(src, dst, n) for (int _ = 0; _ < (n); _++) (dst)[_] = weight2char((src)[_])

// Prm1L の圧縮対応ランダム突然変異
#define randMutPrm1LComp(prp) do {\
    float _pr_arr[PRM1L_LEN];\
    Prm1L2array(prp, _pr_arr);\
    randMutArrayComp(_pr_arr, PRM1L_LEN);\
    array2Prm1L(_pr_arr, prp);\
} while (0)

// types

// 1 middle layer
// including bias
typedef struct prm1L {
    float weight1[PRM1L_L2_NUM][MASU_NUM + 1];
    float weight2[PRM1L_L2_NUM];
} Prm1L;

// MGG のファイルに保存するための型
// 何世代進めたかの情報も保存したい
typedef struct mggPrm1LComp {
    signed char dat[(MASU_NUM + 2) * PRM1L_L2_NUM * MGG_NUM_PRM1L];
    int adv;
} MggPrm1LComp;

// type of function for selection, crossover and mutation
// ソート済み適応度, ソート済み個体番号, 現世代個体配列, 次世代個体配列
typedef void (*scmFuncPrm1L)(const int*, const int*, const Prm1L*, Prm1L*);

// 次の盤面を決定する関数の型
typedef Board (*decNxtPrm1L)(const Board*, int, const Prm1L*);

// 大域変数
// global variables

// Prm1Lの要素数
// マクロだったものを変数に変更
// MASU_NUM と PRM1L_L2_NUM を基に計算
extern const int PRM1L_LEN;

// 圧縮したPrm1L配列の長さ (1要素1byteなので直接サイズとしても使える)
// 最後の要素はソート済みフラグに使う予定
extern const int PRM1L_COMP_LEN;

// functions

// Prm1Lを圧縮して保存するGAを使用するときに
// 最初に行う設定関数
// おまけに盤面も初期化してくれる
void initPrm1LComp(void);

// black: +1, empty: 0, white: -1
// add an element for bias
void board2arraySymmetryPlus(Board src, int dst[MASU_NUM + 1]);

// convert weight array to Prm1L
void array2Prm1L(const float src[PRM1L_LEN], Prm1L *dst);

// パラメータ自体は変更しないのでconstを追加
// convert Prm1L to a weight array
void Prm1L2array(const Prm1L *src, float dst[PRM1L_LEN]);

// show parameters (for Prm1L)
void showPrm1L(Prm1L pr);

// create random parameter (for Prm1L)
void randPrm1L(Prm1L *prp);

// calculate point (with Prm1L)
// the more advantageous to black, the higher the score
float evalWithPrm1L(Board b, const Prm1L *prp);

// Prm1L で評価した中で最良の盤面を取得 (正規化前提)
// 相手のコマが黒なので, 相手目線で評価値が最低の盤面を選ぶ (符号反転しないため)
// 次の盤面の配列, 配列長, Prm1L ポインタ
Board getBoardForBlackPrm1LBest(const Board *next_boards, int n, const Prm1L *prp);

// assume that the next turn is black
// n: the number of next boards
// use Prm1L
// decide next board by roulette
Board getBoardForBlackPrm1LRlt(const Board*, int, const Prm1L*);

// return winner
// give a function pointer to determine the next board (with Prm1L)
// boards are normalized
// 指し手決定関数, 先手パラメータ, 後手パラメータ
int oneToOneNPrm1LFlex(decNxtPrm1L, Prm1L, Prm1L);

// play against random
// return winner
// boards are normalized
// 指し手決定関数, パラメータ, パラメータの手番
int Prm1LVSRandomNormal(decNxtPrm1L, Prm1L, int);

// calculate win rate when playing against random AI
// n: number of games
float calcWinRatePrm1LVSRand(Prm1L pr, int pr_color, int n);

// check win rate when playing against random AI
// n: number of games in each color??
void checkWinRatePrm1LVSRand(Prm1L pr, int n);

// with Prm1L[POPULATION]
// win: +2, draw: +1, lose: 0
// give a function pointer to decide the next board
// 指し手決定関数, 個体配列, 適応度格納配列
void leagueMatchPrm1LFlex(decNxtPrm1L, const Prm1L*, int*);

// ランダム対戦の勝ち点で適応度を決める
// 指し手決定関数, 個体配列, 適応度格納配列, 色ごとの試合数
void evalFitnessPrm1LVSRand (decNxtPrm1L, const Prm1L*, int*, int);

// リーグ戦と対ランダムの両方で適応度を決める
// 指し手決定関数, 個体配列, 適応度格納配列
void evalFitnessHybrid(decNxtPrm1L, const Prm1L*, int*);

// write parameters to a file
// give a file name for writing
// be careful of overwriting
int dumpPrm1LDirect(const char *fname, Prm1L *pra, size_t pra_size);

// パラメータを圧縮して書き込む (ファイル名直接指定)
// 上書きに注意
// フラグを渡す
int dumpPrm1LCompDirect(const char *fname, Prm1L *pra, char flag);

// 圧縮して書き込み (ファイルフォーマットと世代番号を与える)
// フラグを渡す
int dumpPrm1LComp(const char *format, int generation, Prm1L *pra, char flag);

// make first generation file (Prm1L)
// give a file name format
// record all individuals
int makeFGFilePrm1L(const char *format);

// 圧縮バージョンで最初の世代を作成
int makeFGFilePrm1LComp(const char *format);

// MGG 初期世代を作成
// 1つのファイルを書き換えていくスタイルを使いたい
// ファイル名は直接与える
int makeFGFilePrm1LMGGComp(const char*);

// read parameters from a file (Prm1L)
// give a file name directly
int loadPrm1LDirect(const char *fname, Prm1L *pra, size_t pra_size);

// read parameters from a file (Prm1L)
int loadPrm1L(const char *format, int gene_num, Prm1L *pra, size_t pra_size);

// ファイル名直接記入バージョン
int loadPrm1LCompDirect(const char *fname, Prm1L *pra);

// 圧縮されたファイルからPrm1Lの配列を取得
int loadPrm1LComp(const char *format, int gene_num, Prm1L *pra);

// MGG ファイルから, 無作為に2個体を取得
// シードは世代番号で決定
// 返り値は世代数とする
// ファイル名, 番号の配列, 各番号から取り出したパラメータを格納する配列, 番号指定フラグ
int pick2Prm1LMGGComp(const char*, int*, Prm1L*, int);

// MGG ファイルの指定した2つの番号を, 勝ち残った個体に更新
// なんとなく返り値は世代数でいっか
int update2Prm1LMGGComp(const char *fname, const int nums[2], const Prm1L winners[2]);

// load a representative of Prm1L
Prm1L loadRepPrm1L(const char *format, int gene_num, int loc_pop);

// view parematers in a file (Prm1L)
void checkPrm1LFile(const char *format, int gene_num);

// uniform crossover (Prm1L)
// 母のポインタ, 父のポインタ
Prm1L uniCrossPrm1L(const Prm1L*, const Prm1L*);

// uniform crossover (Prm1L)
// including random mutation
Prm1L uniCrossRMPrm1L(Prm1L mother, Prm1L father);

// 指定した番号のブロックをコピー
// コピー元, コピー先のパラメータはそれぞれポインタで与える
void copyBlockPrm1L(const Prm1L*, Prm1L*, int);

// ブロックごとに一様交叉を行う
// weight1[i][], weight2[i] の組み合わせをブロックと考える
// パターンとその重みのペアと見なせる?
Prm1L uniCrossBlockPrm1L(const Prm1L*, const Prm1L*);

// 一点交叉
// 親の引き継ぎ方を入れ替えた2つの子を得る
// 今回は親もポインタで与えることにする (ややこしくてごめん)
void singlePCross(const Prm1L *mother_p, const Prm1L *father_p, Prm1L children[2]);

// 二点交叉 (２人っ子)
// 末尾が交叉点として選ばれた場合, 一点交叉と等価になる
void doublePCross(const Prm1L *mother_p, const Prm1L *father_p, Prm1L children[2]);

// 多点交叉を試してみたい
// 引数に交叉点の数を追加 (一点交叉と二点交叉いらなくなるね)
// 母, 父, 子の配列[2], 交叉点数
void multiPCross(const Prm1L*, const Prm1L*, Prm1L*, int);

// BLX-α 交叉? (Blend Crossover)
// 2つの個体でできる超直方体の内部もしくは周辺に子を作成
// 子は1回につき1つ
Prm1L blendCrossPrm1LComp(const Prm1L*, const Prm1L*);

// ランダム突然変異する
// 既に交叉と突然変異が合体している関数なら不要だが, そうでない場合のため
void randMutPrm1L(Prm1L *prp);

// 交叉関数のデバッグ
void crossTestPrm1L(void);

// roulette selection
// uniform crossover
void rltUniRd(const int *fitness, const int *numbers, const Prm1L *current, Prm1L *next);

// make next generation file
// give a function pointer for selection, crossover and mutation
int nGenePrm1L(scmFuncPrm1L scm, const char *format, int gene_num, int safety);

// 適応度保存用ファイルのファイル名を作る
// .bin の前に _fitness を付けたい
// エラー処理は未定
int makeFitnessFileName(char *dst, size_t dst_size, const char *format, int gene_num);

// 圧縮されたファイルから個体を読み出し, 適応度を評価する
// 適応度降順に個体を並び替え, 同じファイルに書き込む
// 適応度はルーレット選択等に用いるため, 呼び出し元へ配列を渡す
int sortPrm1LCompFileByFitness(const char *fname, int *fitness);

// 次の世代のファイルを作る関数 (圧縮バージョン)
// ついでに適応度評価をした現世代のファイルもソートして書き換える (あとで使えそう)
// ソート済みファイルを使ってルーレット選択をする際, 適応度も必要と考えてファイルに保存
// 再現性確保のためのシードを２つ与えることにする
int nGenePrm1LComp(scmFuncPrm1L scm, const char *format, int gene_num, u_int seed1, u_int seed2, int safety);

// with Prm1L
void nGenePrm1LLoop(scmFuncPrm1L scm, const char *format, int safety, int st, int loop);

// with Prm1L
// use SEED
void nGenePrm1LLoopSeed(scmFuncPrm1L scm, const char *format, int safety, int st, int loop);

// 圧縮版次世代作成関数をループさせる関数
// 引数は開始世代番号と, 終了世代番号に変更 (最終世代はファイル作成のみ)
void nGenePrm1LCompLoop(scmFuncPrm1L scm, const char *format, int safety, int start, int stop);

// 統計値を眺めてみたい
// 主に分散に興味がある
// 各個体を528次元ベクトルと考え, 平均のベクトルを求めたい
// さらにその平均のベクトルからの距離の平均を求めたい
void viewStatPrm1L(Prm1L *pra);

#endif