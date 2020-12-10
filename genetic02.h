#ifndef GENETIC02_H
#define GENETIC02_H

// to use MASU_NUM
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include "othello.h"

#ifndef SEED
#define SEED 123U // シード値
#endif

#define SPRM_LEN 10 // 区別できるマスの数

#define SURVIVE_NUM 10 // 陳腐化したマクロ

#ifndef CMN_RATIO
#define CMN_RATIO 0.9 // ランキング選択で, 等比数列で確率を決定する場合の公比 (調査対象)
#endif

#ifndef ELITE_NUM
#define ELITE_NUM 0 // エリート数
#endif

#ifndef POPULATION
#define POPULATION 50 // 個体数
#endif

#ifndef MUT_RATE
#define MUT_RATE .01 // 突然変異率 (double)
#endif

#ifndef GAME_NUM
#define GAME_NUM 50 // ランダム対戦の色ごとの試合数 (白黒入れ替えるので, 総試合数はこの倍になる)
#endif

// 適応度評価関数を決める識別子
// 1bit目: 指し手関数識別子 (0: 固定, それ以外: ルーレット)
// 2bit目: 対ランダム or リーグ戦 (0: リーグ戦, それ以外: 対ランダム)
#define EF_FUNC_ID 0b10

// parameter sample
#define SAMP_PRM_NUMS 0.50, -0.20, 0.0, -0.02, -0.25, -0.05, -0.05, 0.0, -0.02, -0.02

// file name formats
#define FNF_SPRM_BASE "prm/simple_prm%03d.bin"

// determine the next board by roulette
// roulette selection (use points directly)
#define FNF_RLTRLT "prm/sprm_rltrlt%03d.bin"

// roulette selection
// subtract the minimum value from all points
#define FNF_RR_SUBMIN "prm/sprm_rr_submin%03d.bin"

// ranking selection
// the probability is 100 - rank (relative)
#define FNF_RRANK "prm/sprm_rrank%03d.bin"

// ranking selection
// the probability is 2 ^ rank (relative)
#define FNF_RRANK_EXP "prm/sprm_rrank_exp/sprm_rrank_exp%03d.bin"

// don't evaluate fitness
// use as a comparison target
#define FNF_NOFIT "prm/sprm_nofit/sprm_nofit%03d.bin"

// test file format
#define FNF_TEST "prm/test/test%03d.bin"

// 050: population
// 06: the number of elite
// 00: roulette selection
// 00: uniform crossover
// 00: random mutation
// 005: mutation rate
#define FNF_05006000000005 "prm/sprm050_06_rlt_uni_rd005/sprm050_06_rlt_uni_rd005_g%03d.bin"

// R: 指し手ルーレット (今まで何も付けてなかったけど一応)
// C: 圧縮バージョン
// ファイル名もsprmが長いのでsでよくね?って思った
// ランダムAI対戦を復活させたらどうなるだろう?
// g 付け忘れてた
#define FNF_RC05006000000005 "prm/srltc050_06_rlt_uni_rd005/srltc050_06_rlt_uni_rd005_g%03d.bin"

// select the board with the highest evaluation value
#define FNF_D05006000000005 "prm/sprmdef050_06_rlt_uni_rd005/sprmdef050_06_rlt_uni_rd005_g%03d.bin"

// 01: random selection
// 10: average and uniform crossover (once for each parents)
// 忘れちゃいけないのが, 平均化交叉には突然変異入れてないんだよな
// 実質2.5%?
#define FNF_10010011000005 "prm/sprm100_10_rd_au_rd005/sprm100_10_rd_au_rd005_g%03d.bin"
#define FNF_05006011000005 "prm/sprm050_06_rd_au_rd005/sprm050_06_rd_au_rd005_g%03d.bin"

// 11: 平均と一様交叉半分ずつ. ただし突然変異率はともに等確率とする.
#define FNF_RC05006001100005 "prm/srltc050_06_rlt_aue_rd005/srltc050_06_rlt_aue_rd005_g%03d.bin"

// ルーレット選択, 平均一様, ランダム突変5%
#define FNF_10010001000005 "prm/sprm100_10_rlt_au_rd005/sprm100_10_rlt_au_rd005_g%03d.bin"
#define FNF_05006001000005 "prm/sprm050_06_rlt_au_rd005/sprm050_06_rlt_au_rd005_g%03d.bin"

#define FNF_05006010000005 "prm/sprm050_06_rd_uni_rd005/sprm050_06_rd_uni_rd005_g%03d.bin"

// 02: single point crossover
#define FNF_05006000200005 "prm/sprm050_06_rlt_1p_rd005/sprm050_06_rlt_1p_rd005_g%03d.bin"
// 指し手ルーレット, 圧縮, ルーレット選択, 一点交叉, ラン突5%
#define FNF_RC05006000200005 "prm/srltc050_06_rlt_1p_rd005/srltc050_06_rlt_1p_rd005_g%03d.bin"

// 01: shift mutation (no limit)
#define FNF_05006000001005 "prm/sprm050_06_rlt_uni_sft005/sprm050_06_rlt_uni_sft005_g%03d.bin"

// 02: shift mutation (limited)
#define FNF_05006000002005 "prm/sprm050_06_rlt_uni_sftl005/sprm050_06_rlt_uni_sftl005_g%03d.bin"

// 10: shift mutation (no limit) afte random mutation
// each mutation has a 5% chance of occuring
#define FNF_05006000010005 "prm/sprm050_06_rlt_uni_rdsft005/sprm050_06_rlt_uni_rdsft005_g%03d.bin"

// ランダム選択(01), 一様交叉(00), ランダム突変後制限なしシフト突変(10)
// 指し手ルーレットバージョン
#define FNF_05006010010005 "prm/sprm050_06_rd_uni_rdsft005/sprm050_06_rd_uni_rdsft005_g%03d.bin"
// 指し手固定バージョン
#define FNF_D05006010010005 "prm/sprmdef050_06_rd_uni_rdsft005/sprmdef050_06_rd_uni_rdsft005_g%03d.bin"

// 20: shift mutation (limited) after random mutation
#define FNF_05006000020005 "prm/sprm050_06_rlt_uni_rdsftl005/sprm050_06_rlt_uni_rdsftl005_g%03d.bin"

// 個体数50, エリート6, ランダム突変100%
// ルーレットや一様交叉も0だが, 選択や交叉をしてから突然変異をしたとも考えられる(無理やり)
// やはりアンダースコアで空白を表現した
#define FNF_05006000000100 "prm/sprm050_06___rd100/sprm050_06___rd100_g%03d.bin"

// 圧縮, 個体数50, エリートなし, ランキング選択 (幾何, 比0.90), 一様交叉2, ランダム突然変異1%
// RD: 対ランダム & 指し手固定
// _0290_: 等比数列ランキング選択と, その公比の100倍 (公比1はランダム選択と等価である)
// 05: 一様交叉2人っ子バージョン
#define FNF_RDC05000_0290_0500001 "prm/srddc050__rkg90_uni2_rd001/srddc050__rkg90_uni2_rd001_g%03d.bin"

// choose randomly either a or b
#ifndef alternative
#define alternative(a, b) (rand() & 1 ? (a) : (b))
#endif

// Sprm からマスに対応する値を取り出すためのグローバル変数設定
// convert from an address to the weight index?
#define setCORR_TABLE() for (int _ = 0; _ < MASU_NUM; _++) CORR_TABLE[_] = ad2index(normalAd(_ * 2))

// グローバル変数を表示
#define showCORR_TABLE() for (int _ = 0; _ < 8; _++) printDecimalArray(CORR_TABLE + _ * 8, 8)

// サンプルパラメータを作成
// make a sample of parameters
// set global variable "SAMP_PRM"
#define makeSprmSample() do {float spr[] = {SAMP_PRM_NUMS}; for (int _ = 0; _ < SPRM_LEN; _++) SAMP_PRM.weight[_] = spr[_];} while(0)

// パラメータを一行で表示
// print a simple parameter on one line
#define showSprmOneLine(pr) printFloatArray((pr).weight, SPRM_LEN)

// uniform crossover (array)
#ifndef uniCrossArray
#define uniCrossArray(M, F, C, n) for (int _ = 0; _ < n; _++) (C)[_] = alternative((M)[_], (F)[_])
#endif

// 符号無文字型の乱数を返す
// 代入する変数の型に依存?
#define randUchar() ((rand() & 0xff))

// 圧縮された符号無文字を-0.5から0.5までの小数に変換
#define uchar2weight(uc) (((uc) - 127.5f) / 255)

// 圧縮対応の重み乱数を生成
#define randWeightUchar() uchar2weight(randUchar())

// -0.5から0.5までの小数を符号無文字型に圧縮
// 圧縮精度より小さい値を足せば, 切り捨てられて同じ値になる?
#define weight2uchar(w) (((w) + 0.5000000596f) * 255)

// 符号無文字型の乱数配列を作成
#define randUcharArray(A, n) for (int _ = 0; _ < (n); _++) A[_] = randUchar()

// 符号無文字型配列を重み配列に変換
#define uchar2weightArray(src, dst, n) for (int _ = 0; _ < (n); _++) (dst)[_] = uchar2weight((src)[_])

// 重み配列を符号無文字型配列に変換 (圧縮)
#define weight2ucharArray(src, dst, n) for (int _ = 0; _ < (n); _++) (dst)[_] = weight2uchar((src)[_])

// Sprm配列を圧縮
// 関数でもconstは引っかかったのでマクロに戻す
// できる限りマクロに式を渡さないように一時変数を多用
#define compSprmArray(pra, uca, n) do {\
    const float *_fp;\
    u_char *_ucp;\
    for (int _ = 0; _ < (n); _++) {\
        _fp = (pra)[_].weight;\
        _ucp = (uca) + _ * SPRM_LEN;\
        weight2ucharArray(_fp, _ucp, SPRM_LEN);\
    }\
} while (0)

// 圧縮された配列からSprm配列に展開
// extract という単語を略してみる
#define extrSprmArray(uca, pra, n) do {\
    const u_char *_ucp;\
    float *_fp;\
    for (int _ = 0; _ < (n); _++) {\
        _ucp = (uca) + _ * SPRM_LEN;\
        _fp = (pra)[_].weight;\
        uchar2weightArray(_ucp, _fp, SPRM_LEN);\
    }\
} while (0)

// Sprmの配列を圧縮対応乱数で作成
// n には基本 POPULATION を与えると思うが可変にしておく
// イテレータ変数が被らないように, 各Sprmのポインタを一時変数に代入
// 展開マクロができたのでそれを使う
#define randSprmArrayComp(pra, n) do {\
    int _tl = SPRM_LEN * (n);\
    u_char _uca[_tl];\
    randUcharArray(_uca, _tl);\
    extrSprmArray(_uca, pra, n);\
} while (0)

// Sprm配列を圧縮してファイルに書き込む
// ソート済みか否かのフラグも与える
// 書き込み失敗で抜ける
#define dumpSprmFileCompDirectExit(fname, pra, flag) do {\
    u_char uca[SPRM_FILE_SIZE_COMP];\
    compSprmArray((pra), uca, POPULATION);\
    uca[SPRM_FILE_SIZE_COMP - 1] = (flag);\
    dumpFileDirectExit((fname), uca, SPRM_FILE_SIZE_COMP);\
} while (0)

// シードを追加したフォーマットを作成
// 30は適当な数字
#define formatPlusSeed(format_old, format, format_size) do {\
    int l = strlen(format_old) - 3;\
    char _ex_bin[l];\
    snprintf(_ex_bin, l, "%s", (format_old));\
    snprintf((format), (format_size), "%s_s%03u.bin", _ex_bin, SEED);\
} while (0)

// 配列を突然変異 (圧縮対応乱数限定)
// 一定確率で乱数に書き換え
// 突然変異決定乱数を double 型にして精度up?
#define randMutArrayCC(A, n) for (int _ = 0; _ < (n); _++) if (randDouble() < MUT_RATE) (A)[_] = randWeightUchar()

// Sprmのランダム突然変異 (圧縮対応乱数限定)
// 引数にはSprmのポインタ型を与える
#define randMutSprmCC(prp) do {float *_flp; _flp = (prp)->weight; randMutArrayCC(_flp, SPRM_LEN);} while (0)

// 型の定義
// simple parameter
typedef struct sprm{
    float weight[SPRM_LEN];
} Sprm;

// 次の盤面決定関数の型を定義
// 盤面配列, 長さ, 使うパラメータのポインタを与える
typedef Board (*decNxtSprm)(const Board*, int, const Sprm*);

// the type of function pointer to select, crossover and mutation
typedef void (*scmFunc)(const int*, const int*, const Sprm*, Sprm*);

// ソート済み対応の選択・交叉・突然変異関数
typedef void (*scmSprmSorted)(const int*, const Sprm*, Sprm*);

// 適応度評価関数 (functions to evaluate fitness)
// リーグ戦, ランダム対戦等
// 引数は指し手決定関数ポインタ, 評価対象のSprm配列, 適応度を代入する配列
typedef void (*efSprm)(decNxtSprm, const Sprm*, int*);

// global variables

// マスとSprmの重みの対応表
extern int CORR_TABLE[MASU_NUM];
// 圧縮版Sprmファイルのサイズ (フラグ付き)
extern const int SPRM_FILE_SIZE_COMP;

extern Sprm SAMP_PRM;

// 指し手決定関数 (マクロのIDによって決まる)
extern decNxtSprm DET_FUNC;

// 適応度評価関数 (マクロのIDによって決まる)
extern efSprm EF_FUNC_SPRM;

// シード値に SEED を設定したときの最初に得られる乱数を記録
// シード値が変わってないかチェック (いつ使うのだろう)
extern int FIRST_RAND;

// functions

// グローバル変数等初期化関数
// シード設定も行う
// パラメータも表示
void initSprm(void);

// print a simple parameter
void showSprm(Sprm pr);

// show a part of parameter array
void showFamilyPart(const Sprm*);

// get the smallest value in an array
int getMinArray(const int *A, int n);
// normalize an address
int normalAd(int ad);
// ad: normalized address
int ad2index(int ad);

// set a parameter to a floating point random number from -0.5 to 0.5
void randSprm(Sprm *prp);

// calculate average
Sprm makeChildAverageSprm(Sprm mother, Sprm father);
// give mutant rate
float fcrossMFlex(float a, float b, float rate);

// uniform crossover
// include mutation
Sprm makeChildCrossMSprm(Sprm mother, Sprm father);

// uniform crossover
// no mutation
Sprm uniCrossSprm(Sprm mother, Sprm father);

// shift the value from 0.05 to 0.1
// no limit
void shiftMutNoLim(Sprm *prp);

// shift the value from 0.05 to 0.10
// limit the value between -0.5 and 0.5
void shiftMutLimited(Sprm *prp);

// mutate with a given probability
// otherwise copy
float copyOrMutation(float x, float mut_rate);

// single point crossover
// give 2 parameters, children's array, and mutation rate
// the number of children is 2
void singlePointCrossover(Sprm mother, Sprm father, Sprm children[2], float mut_rate);

// 一様交叉 (2人っ子), 突然変異なし
void uniCrossSprm2C(const Sprm*, const Sprm*, Sprm*);

// 交叉テスト用関数
void crossTestSprm(void);

// caluculate point
float evaluationSimple(Board b, Sprm pr);
// assume that the next turn is black
// n: the number of next boards
// use simple parameter
Board getBestBoardForBlackSimple(const Board *next_boards, int n, const Sprm *prp);

// assume that the next turn is black
// n: the number of next boards
// use simple parameter
// decide next board by roulette
Board getBoardForBlackSimpleRoulette(const Board *next_boards, int n, const Sprm *prp);

// return winner
// give a function pointer as an argument
int oneToOneNormalSprmFlex(decNxtSprm dnfunc, const Sprm *spp, const Sprm *gpp);

// play against random
// return winner
// 引数で指し手決定関数を変更可能にした
// pythonで扱うときにマクロをいちいち変更するのが面倒だった
int SprmVSRandomNormal(decNxtSprm dnfunc, const Sprm *prp, int my_color);

// calculate win rate when playing against random AI
// n: number of games
float calcWinRateSprmVSRand(Sprm pr, int pr_color, int n);

// calculate win rate when playing against random AI
// n: number of games in each color
float calcWinRateSprmVSRandTotal(Sprm pr, int n);

// make first generation file
// give a file name format
// record all individuals!!
int makeFirstGeneFileFlex(const char *format);

// 圧縮版Sprm初期世代ファイルを作成したい
int makeFGFileSprmComp(const char *format);

// ファイル名をそのまま与えてSprmをロード
int loadSprmFileDirect(const char *fname, Sprm *pra, size_t pra_size);

// 圧縮ファイルからロード
// エラーやソート済みフラグを返す
// 返り値が重要な場合はマクロ化しにくいかな?
// 個体数を指定するように変更
int loadSprmFileCompDirect(const char *fname, Sprm *pra, int loc_pop);

// read parameters from a file
int loadSprmFile(const char *format, int gene_num, Sprm *pra, size_t pra_size);

// 圧縮ファイルからロード (フォーマットと世代番号から)
// フォーマット, 世代番号, Sprm配列, 配列長
int loadSprmFileComp(const char*, int, Sprm*, int);

// load representative of Sprm
Sprm loadRepSprm(const char *format, int gene_num, int loc_pop);

// write parameters to a file
// give a file name for writing
// be careful of overwriting
int dumpSprmFileDirect(const char *fname, Sprm *pra, size_t pra_size);

// check parameter in a file
// give the file name format and generation number
void checkSprmFile(const char *format, int gene_num);

// 圧縮ファイルからパラメータを読み取って確認
// ファイル名作成のルーチンを省略したいところだが...
void checkSprmFileComp(const char *format, int gene_num);

// use Sprm[100]
// win: +2, draw: +1, lose: 0
// give a function pointer to decide the next board
void leagueMatchSprmFlex(decNxtSprm, const Sprm*, int*);

// ランダムな手を選ぶ相手との対戦で, 適応度を決める
// 勝点の計算は今まで同様とする
// 試合数を引数で渡す以外は, リーグ戦の引数と同様
void evalFitnessSprmVSRand(decNxtSprm, const Sprm*, int*, int);

// 試合数が固定されたバージョン (fix the nubmer of games)
// リーグ戦関数と型を合わせるため
void evalFitnessSprmVSRandFGN(decNxtSprm, const Sprm*, int*);

// calculate distance
float distSprm(Sprm p1, Sprm p2);

// Sprm の平均値を求める
// 統計値はなんとなく double 型を使う
void calcSprmMeans(const Sprm*, int, double*);

// Sprm の分散を求める (サンプル数 - 1 で割る)
// 引数には平均値も与える
void calcSprmVariances(const Sprm*, int, const double*, double*);

// calculate means and standard deviation from some parameters
void checkSprmStatistics(const Sprm *pra, int nos);

// choose survivors from Sprm[100]
// and show match results
void getSurvivorSprm(Sprm *generation, Sprm *survivors);

// make next generation file
// give a function to choose survivors
int nextGenerationSprmFlex(void (*getSvr)(const Sprm*, Sprm*), const char *format, int gene_num, int safety);

// give a function to loop
void nextGenerationSprmLoopFlex(int (*nGene)(int, int), int safety, int st, int loop);

// select randomly except for elite selection
// perform averaging and uniform crossing once for each parents
void randAveUni(const int *fitness, const int *numbers, const Sprm *current, Sprm *next);

// ルーレット選択, 一様交叉, ランダム突然変異
// ソート済み前提のため個体番号は使わない
void rltUniRdS(const int *, const Sprm*, Sprm*);

// make next generation file
// write all individuals to the file
// give a function pointer for selection and crossing
int nGeneSSAFlex(scmFunc selAndCross, const char *format, int gene_num, int safety);

// give a function to loop and file name format
void nGeneSSALoopFlex(int (*nGeneSSA)(const char*, int, int), const char *format, int safety, int st, int loop);

// give a function pointer for selection and crossover
void nGeneSSAFlexLoop(scmFunc selAndCross, const char *format, int safety, int st, int loop);

// give a function pointer for selection and crossover
// use SEED
void nGeneSSAFlexLoopSeed(scmFunc selAndCross, const char *format, int safety, int st, int loop);

// give a function to loop
void nextGenerationSprmFlexLoopFlex(void (*getSvr)(const Sprm*, Sprm*), int (*nGeneF)(void(), const char*, int, int), const char *format, int safety, int st, int loop);

// copy the first generation
// give the destination file format
void copyFGFlex(const char *dst_format);

// 圧縮されたファイルから個体を読み出し, 適応度を評価する
// 適応度降順に個体を並び替え, 同じファイルに書き込む
// 適応度はルーレット選択等に用いるため, 呼び出し元で配列を渡す
// 次世代作成関数が膨らまないように, ここで適応度の読み書きをする
int sortSprmCompFileByFitness(efSprm, const char*, int*);

// 次の世代のファイルを作る関数 (圧縮バージョン)
// ついでに適応度評価をした現世代のファイルもソートして書き換える
// ソート済みファイルを使ってルーレット選択をする際, 適応度も必要と考えてファイルに保存
// 再現性確保のためのシードを2つ与える
// 個体番号配列を撤廃
int nGeneSprmComp(scmSprmSorted scm, const char *format, int gene_num, u_int seed1, u_int seed2, int safety);

// 圧縮版次世代作成関数をループさせる関数
// 引数は開始世代番号と, 終了世代番号に変更 (最終世代はファイル作成のみ)
void nGeneSprmCompLoop(scmSprmSorted scm, const char *format, int safety, int start, int stop);

#endif