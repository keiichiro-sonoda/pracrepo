#include <math.h>
#include <time.h>
#include <string.h>

#include "genetic01.h"
#include "sort01.h"

// global variables

// Prm1L の要素数
const int PRM1L_LEN = (MASU_NUM + 2) * PRM1L_L2_NUM;

// ファイルの長さ
// ソート済みフラグ追加
const int PRM1L_COMP_LEN = PRM1L_LEN * POPULATION + 1;

// functions

// Prm1Lを圧縮して保存するGAを使用するときに
// 最初に行う設定関数 (ファイル長設定)
// おまけに盤面も初期化してくれる
void initPrm1LComp(void) {
    srand(SEED);
    initBoard();
    printf("population         : %4d\n", POPULATION);
    printf("the number of elite: %4d\n", ELITE_NUM);
    printf("mutation rate      : %4.2f\n", MUT_RATE);
    printf("bias magnification : %4d\n", BIAS_MAG);
    printf("seed               : %4u\n", SEED);
    //printDecimal(PRM1L_LEN);
}

// black: +1, empty: 0, white: -1
// add an element for bias
void board2arraySymmetryPlus(Board src, int dst[MASU_NUM + 1]) {
    board2arraySymmetry(src, dst);
    // for bias
    // ここで倍率を設定する
    dst[MASU_NUM] = BIAS_MAG;
}

// コピー元は変更しない
// convert weight array to Prm1L
void array2Prm1L(const float src[PRM1L_LEN], Prm1L *dst) {
    int c = 0;
    for (int i = 0; i < PRM1L_L2_NUM; i++) {
        for (int j = 0; j <= MASU_NUM; j++) {
            dst->weight1[i][j] = src[c];
            c++;
        }
    }
    for (int i = 0; i < PRM1L_L2_NUM; i++) {
        dst->weight2[i] = src[c];
        c++;
    }
}

// パラメータ自体は変更しないのでconstを追加 (参照渡しなので)
// convert Prm1L to a weight array
void Prm1L2array(const Prm1L *src, float dst[PRM1L_LEN]) {
    int c = 0;
    for (int i = 0; i < PRM1L_L2_NUM; i++) {
        for (int j = 0; j <= MASU_NUM; j++) {
            dst[c] = src->weight1[i][j];
            c++;
        }
    }
    for (int i = 0; i < PRM1L_L2_NUM; i++) {
        dst[c] = src->weight2[i];
        c++;
    }
}

// show parameters (for Prm1L)
void showPrm1L(Prm1L pr) {
    printString("weight1:");
    for (int i = 0; i < PRM1L_L2_NUM; i++) {
        //printFloatArray(pr.weight1[i], MASU_NUM + 1);
        printFloatArrayPart(pr.weight1[i], MASU_NUM + 1);
    }
    printString("weight2:");
    printFloatArray(pr.weight2, PRM1L_L2_NUM);
}

// create random parameters (for Prm1L)
void randPrm1L(Prm1L *prp) {
    float tmp[PRM1L_LEN];
    randWeightArray(tmp, PRM1L_LEN);
    array2Prm1L(tmp, prp);
}

// calculate point (with Prm1L)
// the more advantageous to black, the higher the score
float evalWithPrm1L(Board b, Prm1L pr) {
    int i, j, inputs[MASU_NUM + 1];
    // middle points
    float pa1[PRM1L_L2_NUM];
    // output point
    float output = .0f;
    // convert board to array
    board2arraySymmetryPlus(b, inputs);
    // 第2層への出力を初期化
    zeros(pa1, PRM1L_L2_NUM);
    // first layer
    for (i = 0; i < PRM1L_L2_NUM; i++) {
        for (j = 0; j <= MASU_NUM; j++) {
            pa1[i] += inputs[j] * pr.weight1[i][j];
        }
        // activate
        pa1[i] = ACT_FUNC(pa1[i]);
    }
    //printFloatArray(pa1, 8);
    // output layer
    for (i = 0; i < PRM1L_L2_NUM; i++)
        output += pa1[i] * pr.weight2[i];
    return output;
}

// Prm1L で評価した中で最良の盤面を取得 (正規化前提)
// 相手のコマが黒なので, 相手目線で評価値が最低の盤面を選ぶ (符号反転しないため)
// 次の盤面の配列, 配列長, Prm1L ポインタ
Board getBoardForBlackPrm1LBest(const Board *next_boards, int n, const Prm1L *prp) {
    // 最良 (悪?) の盤面の添字を格納する
    int worst_board_ind;
    float pt, min_pt;
    // floatで表現できる最大値で初期化
    min_pt = FLT_MAX;
    for (int i = 0; i < n; i++) {
        // 最小値を下回ったら更新
        if ((pt = evalWithPrm1L(next_boards[i], *prp)) < min_pt) {
            min_pt = pt;
            worst_board_ind = i;
        }
    }
    return next_boards[worst_board_ind];
}

// assume that the next turn is black
// n: the number of next boards
// use Prm1L
// decide next board by roulette
Board getBoardForBlackPrm1LRlt(Board *next_boards, int n, Prm1L pr) {
    float exp_points[n];
    for (int i = 0; i < n; i++) {
        // evaluate all next boards
        // and calculate the power of e (to make numbers positive)
        // sign inversion!
        exp_points[i] = expf(-evalWithPrm1L(next_boards[i], pr) * 10);
    }
    return next_boards[rouletteFloat(exp_points, n, sumFloat(exp_points, n))];
}

// return winner
// give a function pointer to determine the next board (with Prm1L)
// boards are normalized
int oneToOneNPrm1LFlex(Board (*decNxt)(Board*, int, Prm1L), Prm1L spr, Prm1L gpr) {
    Board nba[NEXT_MAX];
    int kc[3];
    int pass = 0;
    int n, dif;
    // set turn
    int turn = 0b01;
    // set initial board
    // 初期盤面が正規化されていないため, 正規化必須
    Board main_board = normalBoard(START);
    while (1) {
        // calculate next
        n = nextBoardNormal2(main_board, nba, kc);
        //showBoard(main_board);
        // can't put a piece anywhere
        if (n == 0) {
            // can't do anything one another
            if (pass) {
                //printf("end\n");
                break;
            }
            // pass
            //printf("pass\n");
            swapNormalizeBoard(&main_board);
            turn ^= 0b11;
            pass = 1;
            continue;
        }
        pass = 0;
        // determine a next board
        // black (first)
        if (turn == 0b01) {
            //printf("black\n");
            main_board = decNxt(nba, n, spr);
        } // white (second)
        else {
            //printf("white\n");
            main_board = decNxt(nba, n, gpr);
        }
        // switch turn
        turn ^= 0b11;
    }
    // difference between black and white
    dif = kc[1] - kc[2];
    if (dif > 0) return turn;
    if (dif < 0) return turn ^ 0b11;
    // draw
    return 0;
}

// play against random
// return winner
// boards are normalized
int Prm1LVSRandomNormal(Prm1L pr, int my_color) {
    Board nba[NEXT_MAX];
    int kc[3];
    int pass = 0;
    int n, dif;
    // set turn
    int turn = 0b01;
    // set initial board
    Board main_board = START;
    while (1) {
        // calculate next
        n = nextBoardNormal2(main_board, nba, kc);
        //showBoard(main_board);
        // can't put a piece anywhere
        if (n == 0) {
            // can't do anything one another
            if (pass) {
                //printf("end\n");
                break;
            }
            // pass
            //printf("pass\n");
            swapNormalizeBoard(&main_board);
            turn ^= 0b11;
            pass = 1;
            continue;
        }
        pass = 0;
        // determine a next board
        // Prm1L's turn
        if (turn == my_color) {
            // determine the next board by roulette
            //printf("black\n");
            main_board = getBoardForBlackPrm1LRlt(nba, n, pr);
        } // random
        else {
            //printf("white\n");
            main_board = nba[rand() % n];
        }
        // switch turn
        turn ^= 0b11;
    }
    // difference between black and white
    dif = kc[1] - kc[2];
    if (dif > 0) return turn;
    if (dif < 0) return turn ^ 0b11;
    // draw
    return 0;
}

// calculate win rate when playing against random AI
// n: number of games
float calcWinRatePrm1LVSRand(Prm1L pr, int pr_color, int n) {
    int count = 0;
    for (int i = 0; i < n; i++)
        if (Prm1LVSRandomNormal(pr, pr_color) == pr_color)
            count++;
    return (float)count / n;
}

// check win rate when playing against random AI
// n: number of games in each color??
void checkWinRatePrm1LVSRand(Prm1L pr, int n) {
    float rb, rw;
    rb = calcWinRatePrm1LVSRand(pr, 0b01, n);
    rw = calcWinRatePrm1LVSRand(pr, 0b10, n);
    //printf("the number of games: %d x 2\n", n);
    printf("win rate (black, white, total): %4.1f%%, %4.1f%%, %4.1f%%\n", rb * 100, rw * 100, (rb + rw) * 50);
}

// with Prm1L[POPULATION]
// win: +2, draw: +1, lose: 0
// give a function pointer to decide the next board
void leagueMatchPrm1LFlex(Board (*decNxt)(Board*, int, Prm1L), const Prm1L *family, int *result) {
    // set all elements to zero
    zeros(result, POPULATION);
    // black index
    for (int i = 0; i < POPULATION; i++) {
        //printf("\ai = %d / %d", i, FAMILY_NUM);
        // white index
        for (int j = 0; j < POPULATION; j++) {
            if (i == j) continue;
            switch(oneToOneNPrm1LFlex(decNxt, family[i], family[j])) {
                // black won
                case 1:
                    result[i] += 2;
                    break;
                // white won
                case 2:
                    result[j] += 2;
                    break;
                // draw
                default:
                    result[i]++;
                    result[j]++;
            }
        }
    }
}

// write parameters to a file
// give a file name for writing
// be careful of overwriting
int dumpPrm1LDirect(const char *fname, Prm1L *pra, size_t pra_size) {
    FILE *fp;
    if ((fp = fopen(fname, "wb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fname);
        return -1;
    }
    fwrite(pra, pra_size, 1, fp);
    fclose(fp);
    return 0;
}

// パラメータを圧縮して書き込む (ファイル名直接指定)
// 上書きに注意
// フラグを渡す
int dumpPrm1LCompDirect(const char *fname, Prm1L *pra, char flag) {
    FILE *fp;
    float w_arr[PRM1L_COMP_LEN - 1];
    // フラグを含む配列を用意
    char comp_pra[PRM1L_COMP_LEN];
    // Prm1L配列を重みの一次元配列にまとめる
    for (int i = 0; i < POPULATION; i++) {
        Prm1L2array(pra + i, w_arr + i * PRM1L_LEN);
    }
    // char型に圧縮
    weight2charArray(w_arr, comp_pra, PRM1L_COMP_LEN - 1);
    // 末尾にフラグを代入
    comp_pra[PRM1L_COMP_LEN - 1] = flag;
    // 書き込み用で開く
    if  ((fp = fopen(fname, "wb")) == NULL) {
        // 失敗
        printf("%s can't be opened.\n", fname);
        return -1;
    }
    // 成功
    fwrite(comp_pra, PRM1L_COMP_LEN, 1, fp);
    fclose(fp);
    return 0;
}

// 圧縮して書き込み (ファイルフォーマットと世代番号を与える)
// フラグを渡す
int dumpPrm1LComp(const char *format, int generation, Prm1L *pra, char flag) {
    char fnamew[FILENAME_MAX];
    snprintf(fnamew, FILENAME_MAX, format, generation);
    return dumpPrm1LCompDirect(fnamew, pra, flag);
}

// make first generation file (Prm1L)
// give a file name format
// record all individuals
int makeFGFilePrm1L(const char *format) {
    char fnamew[FILENAME_MAX];
    snprintf(fnamew, FILENAME_MAX, format, 0);
    // check existence
    // 上書き禁止なら抜ける
    warnOverwritingExit(fnamew);
    // make new family randomly
    Prm1L pra[POPULATION];
    for (int i = 0; i < POPULATION; i++)
        randPrm1L(pra + i);
    // write
    if (dumpPrm1LDirect(fnamew, pra, sizeof pra) < 0)
        return -1;
    printf("%ld bytes were written\n", sizeof pra);
    return 0;
}

// 圧縮バージョンで最初の世代を作成
// float型を仲介せずに, ただchar型乱数配列を書き込む
int makeFGFilePrm1LComp(const char *format) {
    char fnamew[FILENAME_MAX];
    snprintf(fnamew, FILENAME_MAX, format, 0);
    // 上書き注意, 勝手に抜ける
    warnOverwritingExit(fnamew);
    FILE *fp;
    // 圧縮されたPrm1Lの配列を格納
    // 一次元配列として扱う
    char comp_pra[PRM1L_COMP_LEN];
    // -127から127のchar型乱数を代入 (フラグ部分以外)
    randWeightCharArray(comp_pra, PRM1L_COMP_LEN - 1);
    // フラグは0で初期化
    comp_pra[PRM1L_COMP_LEN - 1] = 0;
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        // 失敗
        printf("%s can't be opened.\n", fnamew);
        return -1;
    }
    // 書き込んで閉じる
    fwrite(comp_pra, PRM1L_COMP_LEN, 1, fp);
    fclose(fp);
    // 書き込まれたサイズを表示
    printf("%ld bytes were written\n", sizeof comp_pra);
    return 0;
}

// read parameters from a file (Prm1L)
// give a file name directly
int loadPrm1LDirect(const char *fname, Prm1L *pra, size_t pra_size) {
    FILE *fp;
    if ((fp = fopen(fname, "rb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fname);
        return -1;
    }
    fread(pra, pra_size, 1, fp);
    fclose(fp);
    return 0;
}

// read parameters from a file (Prm1L)
int loadPrm1L(const char *format, int gene_num, Prm1L *pra, size_t pra_size) {
    char fnamer[FILENAME_MAX];
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    return loadPrm1LDirect(fnamer, pra, pra_size);
}

// 圧縮ファイルをロード
// ファイル名直接記入バージョン
// 読み込めた場合, ソートフラグを返す
int loadPrm1LCompDirect(const char *fname, Prm1L *pra) {
    FILE *fp;
    if ((fp = fopen(fname, "rb")) == NULL) {
        // 失敗
        printf("%s can't be opened.\n", fname);
        return -1;
    }
    // 成功
    char comp_pra[PRM1L_COMP_LEN];
    fread(comp_pra, PRM1L_COMP_LEN, 1, fp);
    fclose(fp);
    // フラグ部分を除く
    float w_arr[PRM1L_COMP_LEN - 1];
    // 重み配列に変換
    char2weightArray(comp_pra, w_arr, PRM1L_COMP_LEN - 1);
    // 配列を個体数に分割してPrm1L配列に代入
    for (int i = 0; i < POPULATION; i++) {
        // ポインタをPrm1Lの長さだけずらす
        array2Prm1L(w_arr + i * PRM1L_LEN, pra + i);
    }
    // ソートフラグを返す (char型なので一応intにする)
    return (int)(comp_pra[PRM1L_COMP_LEN - 1]);
}

// 圧縮されたファイルからPrm1Lの配列を取得
int loadPrm1LComp(const char *format, int gene_num, Prm1L *pra) {
    char fnamer[FILENAME_MAX];
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    return loadPrm1LCompDirect(fnamer, pra);
}

// load a representative of Prm1L
Prm1L loadRepPrm1L(const char *format, int gene_num, int loc_pop) {
    Prm1L pra[loc_pop];
    if (loadPrm1L(format, gene_num, pra, sizeof pra) < 0) {
        // failed
        // set random parameters
        randPrm1L(pra);
    }
    return *pra;
}

// view parematers in a file (Prm1L)
void checkPrm1LFile(const char *format, int gene_num) {
    Prm1L pra[POPULATION];
    if (loadPrm1L(format, gene_num, pra, sizeof pra) < 0)
        return;
    // show only beggining and end
    showPrm1L(pra[0]);
    showPrm1L(pra[POPULATION - 1]);
}

// uniform crossover (Prm1L)
Prm1L uniCrossPrm1L(Prm1L mother, Prm1L father) {
    float m_arr[PRM1L_LEN], f_arr[PRM1L_LEN], c_arr[PRM1L_LEN];
    Prm1L child;
    // convert to array
    Prm1L2array(&mother, m_arr);
    Prm1L2array(&father, f_arr);
    // uniform cross over
    uniCrossArray(m_arr, f_arr, c_arr, PRM1L_LEN);
    // convert to Prm1L
    array2Prm1L(c_arr, &child);
    return child;
}

// uniform crossover (Prm1L)
// including random mutation
Prm1L uniCrossRMPrm1L(Prm1L mother, Prm1L father) {
    float m_arr[PRM1L_LEN], f_arr[PRM1L_LEN], c_arr[PRM1L_LEN];
    Prm1L child;
    // convert to array
    Prm1L2array(&mother, m_arr);
    Prm1L2array(&father, f_arr);
    // uniform cross over
    uniCrossArray(m_arr, f_arr, c_arr, PRM1L_LEN);
    // random mutation
    randMutArray(c_arr, PRM1L_LEN);
    // convert to Prm1L
    array2Prm1L(c_arr, &child);
    return child;
}

// 指定した番号のブロックをコピー
// コピー元, コピー先のパラメータはそれぞれポインタで与える
void copyBlockPrm1L(Prm1L *src, Prm1L *dst, int bl_num) {
    // パターンをコピー
    copyArray(src->weight1[bl_num], dst->weight1[bl_num], MASU_NUM + 1);
    // 対応する重みをコピー
    dst->weight2[bl_num] = src->weight2[bl_num];
}

// ブロックごとに一様交叉を行う
// weight1[i][], weight2[i] の組み合わせをブロックと考える
// パターンとその重みのペアと見なせる?
Prm1L uniCrossBlockPrm1L(Prm1L mother, Prm1L father) {
    Prm1L child;
    randPrm1L(&child);
    // レイヤ2の数くり返し
    for (int i = 0; i < PRM1L_L2_NUM; i++) {
        // 50%の抽選で1が出た場合, motherから引き継ぎ
        if (randBit()) {
            copyBlockPrm1L(&mother, &child, i);
        }
        // 0が出た場合, fatherから引き継ぎ
        else {
            copyBlockPrm1L(&father, &child, i);
        }
    }
    return child;
}

// 一点交叉
// 親の引き継ぎ方を入れ替えた2つの子を得る
// 今回は親もポインタで与えることにする (ややこしくてごめん)
void singlePCross(const Prm1L *mother_p, const Prm1L *father_p, Prm1L children[2]) {
    // 親は2次元配列にコピー
    // 子は作った直後にパラメータに変換するため, 一時置き場と考える
    float p_arr[2][PRM1L_LEN], c_arr[PRM1L_LEN];
    Prm1L2array(mother_p, p_arr[0]);
    Prm1L2array(father_p, p_arr[1]);
    // 0からパラメータの末尾の手前までの乱数を交叉点とする
    int cp = randInt(PRM1L_LEN - 1);
    // 子を1つずつ作成
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < PRM1L_LEN; i++) {
            // 比較演算の結果を添字計算に適用にする
            // 0 か 1 を返すなら問題ないはず
            c_arr[i] = p_arr[j ^ (i > cp)][i];
        }
        // パラメータに変換
        array2Prm1L(c_arr, children + j);
    }
}

// 二点交叉 (２人っ子)
// 末尾が交叉点として選ばれた場合, 一点交叉と等価になる
void doublePCross(const Prm1L *mother_p, const Prm1L *father_p, Prm1L children[2]) {
    float p_arr[2][PRM1L_LEN], c_arr[PRM1L_LEN];
    // 交叉点配列
    int cpa[2];
    Prm1L2array(mother_p, p_arr[0]);
    Prm1L2array(father_p, p_arr[1]);
    // 0からパラメータの末尾まで, 2点を重複なしでランダムに選ぶ
    randIntDoubleDep(cpa, 0, PRM1L_LEN - 1);
    // ソート
    randomizedQuicksortAll(cpa, 2);
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < PRM1L_LEN; i++) {
            // 小交叉点より大きく, 大交叉点以下なら交換
            c_arr[i] = p_arr[j ^ ((cpa[0] < i)  && (i <= cpa[1]))][i];
        }
        array2Prm1L(c_arr, children + j);
    }
}

// 多点交叉
// 引数に交叉点の数を追加 (一点交叉と二点交叉いらなくなるね)
void multiPCross(const Prm1L *mother_p, const Prm1L *father_p, Prm1L children[2], int cp_num) {
    float p_arr[2][PRM1L_LEN], c_arr[2][PRM1L_LEN];
    int i, j, k, flag, cp_look, cpa[cp_num + 1];
    Prm1L2array(mother_p, p_arr[0]);
    Prm1L2array(father_p, p_arr[1]);
    // 0からパラメータの末尾-1まで, 交叉点を指定した数だけ重複なしでランダムに選ぶ
    randIntMltDep(cpa, cp_num, 0, PRM1L_LEN - 2);
    // 交叉点を昇順にソート
    randomizedQuicksortAll(cpa, cp_num);
    // 末尾は番兵 (最後に意味のない交叉点を用意)
    cpa[cp_num] = 0x7fffffff;
    // 確認する交叉点をセット (添字もセット)
    cp_look = cpa[k = 0];
    // 1 のときは入れ替えで値を引き継ぐ
    flag = 0;
    for (i = 0; i < PRM1L_LEN; i++) {
        // 交叉点を超えたら, 引き継ぎ対象を入れ替え, 次の交叉点を見る
        if (i > cp_look) {
            flag ^= 1;
            cp_look = cpa[++k];
        }
        for (j = 0; j < 2; j++) {
            // それぞれ引き継ぎ
            c_arr[j][i] = p_arr[j ^ flag][i];
        }
    }
    // Prm1L に戻す
    for (j = 0; j < 2; j++) {
        array2Prm1L(c_arr[j], children + j);
    }
}

// BLX-α 交叉? (Blend Crossover)
// 2つの個体でできる超直方体の内部もしくは周辺に子を作成
// 子は1回につき1つ
// 圧縮対応の値に変換する
Prm1L blendCrossPrm1LComp(const Prm1L *mother_p, const Prm1L *father_p) {
    Prm1L child;
    float d, tmp, min_a, max_a, p_arr[2][PRM1L_LEN], c_arr[PRM1L_LEN];
    Prm1L2array(mother_p, p_arr[0]);
    Prm1L2array(father_p, p_arr[1]);
    for (int i = 0; i < PRM1L_LEN; i++) {
        // 代入しつつ比較
        if ((min_a = p_arr[0][i]) > (max_a = p_arr[1][i]))
            // 母の値が大きければ交換
            swap(float, min_a, max_a);
        d = max_a - min_a;
        // 範囲を少し引き伸ばす
        max_a += d * ALPHA_BLX;
        min_a -= d * ALPHA_BLX;
        tmp = randDoubleRange(min_a, max_a);
        // 値を範囲内に収める
        tmp = clamp(tmp, -0.5f, 0.5f);
        // 圧縮しても値が変わらないようにする
        c_arr[i] = char2weight(weight2char(tmp));
    }
    array2Prm1L(c_arr, &child);
    return child;
}

// ランダム突然変異する
// 既に交叉と突然変異が合体している関数なら不要だが, そうでない場合のため
void randMutPrm1L(Prm1L *prp) {
    float tmp[PRM1L_LEN];
    // 配列に変換
    Prm1L2array(prp, tmp);
    // 突然変異率は各値ごと MUT_RATE に依存
    randMutArray(tmp, PRM1L_LEN);
    // 引数のポインタに代入し直す
    array2Prm1L(tmp, prp);
}

// 交叉関数のデバッグ
void crossTestPrm1L(void) {
    //srand((unsigned)time(NULL));
    Prm1L mother, father, children[2];
    float m_arr[PRM1L_LEN], f_arr[PRM1L_LEN];
    initArrayConst(m_arr, PRM1L_LEN, -0.5f);
    initArrayConst(f_arr, PRM1L_LEN, .5f);
    array2Prm1L(m_arr, &mother);
    array2Prm1L(f_arr, &father);
    //showPrm1L(mother);
    //showPrm1L(father);
    //doublePCross(&mother, &father, children);
    //multiPCross(&mother, &father, children, 4);
    children[0] = blendCrossPrm1LComp(&mother, &father);
    showPrm1L(children[0]);
    //showPrm1L(children[1]);
}

// roulette selection
// uniform crossover
// random mutation
void rltUniRd(const int *fitness, const int *numbers, const Prm1L *current, Prm1L *next) {
    int count = ELITE_NUM;
    int parents[2];
    while (count < POPULATION) {
        rouletteIntMltDep(fitness, POPULATION, parents, 2);
        //printf("%d, %d, %d, %d\n", parents[0], parents[1], numbers[parents[0]], numbers[parents[1]]);
        next[count] = uniCrossRMPrm1L(current[numbers[parents[0]]], current[numbers[parents[1]]]);
        count++;
    }
}

// make next generation file
// give a function pointer for selection, crossover and mutation
int nGenePrm1L(scmFuncPrm1L scm, const char *format, int gene_num, int safety) {
    char fname[FILENAME_MAX];
    // the file name to be read
    snprintf(fname, FILENAME_MAX, format, gene_num);
    printf("read file : %s\n", fname);
    // current family
    Prm1L current[POPULATION];
    // load data
    if (loadPrm1LDirect(fname, current, sizeof current) < 0)
        return -1;
    // the file name to be written
    snprintf(fname, FILENAME_MAX, format, gene_num + 1);
    printf("write file: %s\n", fname);
    // don't allow overwriting
    // 上書きが嫌なら戻る
    if (safety) warnOverwritingExit(fname);
    // next family
    Prm1L next[POPULATION];
    // the array to store points
    int fitness[POPULATION];
    // individual numbers
    int numbers[POPULATION];
    // numbers = {0, 1, 2, ...}
    indices(numbers, POPULATION);
    // evaluate fitness (with Prm1L)
    // the next board is decided by roulette
    leagueMatchPrm1LFlex(getBoardForBlackPrm1LRlt, current, fitness);
    // sort (descending order)
    randomizedQuicksortDDAll(fitness, numbers, POPULATION);
    // show the all or part of results
    printString("results:");
    printDecimalArray(fitness, POPULATION);
    //printDecimalArrayPart(fitness, POPULATION);
    // elite selection
    for (int i = 0; i < ELITE_NUM; i++)
        next[i] = current[numbers[i]];
    // selection and crossing
    scm(fitness, numbers, current, next);
    // view the part of top parameter
    printString("the top of this generation:");
    showPrm1L(next[0]);
    // 逐一勝率チェックしたい場合はこれを実行
    //checkWinRatePrm1LVSRand(next[0], 500);
    // write next family to the file
    // and return error flag
    return dumpPrm1LDirect(fname, next, sizeof next);
}

// 適応度保存用ファイルのファイル名を作る
// .bin の前に _fitness を付けたい
// エラー処理は未定
int makeFitnessFileName(char *dst, size_t dst_size, const char *format, int gene_num) {
    // まずはオリジナルのファイル名から作る
    char fnameo[FILENAME_MAX];
    // フォーマットと世代番号を合わせる
    snprintf(fnameo, FILENAME_MAX, format, gene_num);
    // あとはダイレクトにおまかせ (エラー処理まで)
    makeFitnessFileNameDirectExit(dst, dst_size, fnameo);
    return 0;
}

// 圧縮されたファイルから個体を読み出し, 適応度を評価する
// 適応度降順に個体を並び替え, 同じファイルに書き込む
// 適応度はルーレット選択等に用いるため, 呼び出し元で配列を渡す
// 次世代作成関数が膨らまないように, ここで適応度の読み書きをする
int sortPrm1LCompFileByFitness(const char *fname, int *fitness) {
    // ソート前とソート後のパラメータ配列を用意する (メモリの無駄遣いかな?)
    Prm1L pra1[POPULATION], pra2[POPULATION];
    // 適応度ファイル名
    char fnamef[FILENAME_MAX];
    // 万が一のファイル名オーバーフロー対策
    makeFitnessFileNameDirectExit(fnamef, FILENAME_MAX, fname);
    // ロードしてフラグを取得
    int flag = loadPrm1LCompDirect(fname, pra1);
    // エラーなら-1を返す
    if (flag < 0) return -1;
    // ソート済みなら適応度ファイルを読み込む
    if (flag == 1) {
        // 読み込みエラーなら勝手に抜ける
        loadFitnessShortDirectExit(fnamef, fitness, POPULATION);
        return 1;
    }
    // 個体番号を割り振る
    int numbers[POPULATION];
    indices(numbers, POPULATION);
    // リーグ戦 (指し手ルーレット)
    // 現状で適応度評価はこれだけ
    leagueMatchPrm1LFlex(getBoardForBlackPrm1LRlt, pra1, fitness);
    // 適応度を基に個体番号も同時にソート
    randomizedQuicksortDDAll(fitness, numbers, POPULATION);
    // 適応度順に並び替えてpra2に代入
    for (int i = 0; i < POPULATION; i++)
        pra2[i] = pra1[numbers[i]];
    // ソート後の配列を同じファイルに書き戻す (ソート済みフラグを立てる)
    if (dumpPrm1LCompDirect(fname, pra2, 1)  < 0)
        return -1;
    // 適応度書き込み
    dumpFitnessShortDirectExit(fnamef, fitness, POPULATION);
    return 0;
}

// 次の世代のファイルを作る関数 (圧縮バージョン)
// ついでに適応度評価をした現世代のファイルもソートして書き換える (あとで使えそう)
// ソート済みファイルを使ってルーレット選択をする際, 適応度も必要と考えてファイルに保存
// 再現性確保のためのシードを2つ与えることにする
int nGenePrm1LComp(scmFuncPrm1L scm, const char *format, int gene_num, u_int seed1, u_int seed2, int safety) {
    // 読み込み (ソート) 用と書き込み用ファイル名
    char fnames[FILENAME_MAX], fnamew[FILENAME_MAX];
    snprintf(fnames, FILENAME_MAX, format, gene_num);
    snprintf(fnamew, FILENAME_MAX, format, gene_num + 1);
    printf("sort file: %s\n", fnames);
    printf("new file : %s\n", fnamew);
    // 上書き拒否
    if (safety) warnOverwritingExit(fnamew);
    int fitness[POPULATION];
    // 対戦・ソート用シード
    srand(seed1);
    // 適応度評価とファイルのソート
    // さらに適応度ファイルの読み書きも行う
    int flag = sortPrm1LCompFileByFitness(fnames, fitness);
    // エラー
    if (flag < 0) return -1;
    // ソート済み
    if (flag == 1) {
        printf("%s is already sorted!\n", fnames);
    }
    // 選択・交叉・突然変異用シード
    srand(seed2);
    // なんとなくソート済み適応度を表示
    printDecimalArray(fitness, POPULATION);
    // 今世代と次世代の個体配列を宣言
    Prm1L current[POPULATION], next[POPULATION];
    // 個体配列自体がソートされているため不要だが, 旧仕様との互換性のために定義
    int numbers[POPULATION];
    indices(numbers, POPULATION);
    // ソート済み配列を読み込む
    if (loadPrm1LCompDirect(fnames, current) < 0)
        return -1;
    // トップパラメータを見る
    printString("the top of this generation:");
    showPrm1L(current[0]);
    // エリートはそのままコピー
    copyArray(current, next, ELITE_NUM);
    // 選択, 交叉, 突然変異
    scm(fitness, numbers, current, next);
    // 統計調査
    viewStatPrm1L(current);
    // 乱数に影響が出ないように次世代を作ったら勝率計算 (たまーに)
    // ループ関数じゃなくてこっちでやったほうが手間が少なかった
    if (!(gene_num % 20)) {
        kugiri(100);
        int game_num = 500;
        printf("the number of games: %d x 2\n", game_num);
        printf("the strongest:\n");
        checkWinRatePrm1LVSRand(current[0], game_num);
        printf("the weakest:\n");
        checkWinRatePrm1LVSRand(current[POPULATION - 1], game_num);
    }
    // ソート済みフラグは立てずに書き込み
    return dumpPrm1LCompDirect(fnamew, next, 0);
}

// with Prm1L
void nGenePrm1LLoop(scmFuncPrm1L scm, const char *format, int safety, int st, int loop) {
    time_t t0, t1;
    // get start time
    time(&t0);
    Prm1L rep_pr;
    for (int i = st; i < st + loop; i++) {
        // set the generation number as the seed
        srand((unsigned)i);
        if (nGenePrm1L(scm, format, i, safety) < 0) {
            // error
            printString("error");
            return;
        }
        // たまに勝率計算
        if (i % 20 == 0) {
            kugiri(100);
            rep_pr = loadRepPrm1L(format, i, POPULATION);
            checkWinRatePrm1LVSRand(rep_pr, 500);
        }
        // get time
        time(&t1);
        printf("elapsed time: %lds\n", t1 - t0);
        kugiri(100);
    }
}

// with Prm1L
// use SEED
void nGenePrm1LLoopSeed(scmFuncPrm1L scm, const char *format, int safety, int st, int loop) {
    time_t t0, t1;
    // get start time
    time(&t0);
    unsigned int s1;
    // たまに使うパラメータを代入する変数
    Prm1L rep_pr;
    for (int i = st; i < st + loop; i++) {
        // use the number of generation as a part of seed
        // set seed
        printf("seed: %d\n", s1 = i * SEED);
        srand((unsigned)s1);
        if (nGenePrm1L(scm, format, i, safety) < 0) {
            // error
            printString("error");
            return;
        }
        // たまに勝率計算
        if (i % 20 == 0) {
            kugiri(100);
            rep_pr = loadRepPrm1L(format, i, POPULATION);
            checkWinRatePrm1LVSRand(rep_pr, 500);
        }
        // get time
        time(&t1);
        printf("elapsed time: %lds\n", t1 - t0);
        kugiri(100);
    }
}

// 圧縮版次世代作成関数をループさせる関数
// 引数は開始世代番号と, 終了世代番号に変更 (最終世代はファイル作成のみ)
void nGenePrm1LCompLoop(scmFuncPrm1L scm, const char *format, int safety, int start, int stop) {
    time_t t_arr[2];
    time(t_arr);
    u_int s1, s2;
    for (int gene_num = start; gene_num < stop; gene_num++) {
        // 1 はなんとなくのオフセット
        s1 = SEED + gene_num + 1;
        srand(s1);
        // s2はs1に依存する
        // 排他的論理和で乱数改変
        // 和を計算したときのオーバーフローを回避?
        s2 = rand() ^ SEED;
        printf("seed1: %d, seed2: %d\n", s1, s2);
        // 次の世代へ!
        if (nGenePrm1LComp(scm, format, gene_num, s1, s2, safety) < 0)
            return;
        // get time
        time(t_arr + 1);
        printf("elapsed time: %lds\n", t_arr[1] - t_arr[0]);
        kugiri(100);
    }
}

// 統計値を眺めてみたい
// 主に分散に興味がある
// 各個体を528次元ベクトルと考え, 平均のベクトルを求めたい
// さらにその平均のベクトルからの距離の平均を求めたい
void viewStatPrm1L(Prm1L *pra) {
    int i, j;
    float arr_2d[POPULATION][PRM1L_LEN], means[PRM1L_LEN], dists[POPULATION];
    // 各個体を2次元配列に格納
    for (i = 0; i < POPULATION; i++) {
        Prm1L2array(pra + i, arr_2d[i]);
    }
    // 初期化
    zerosFloat(means, PRM1L_LEN);
    // まずは和を計算
    for (i = 0; i < POPULATION; i++) {
        for (j = 0; j < PRM1L_LEN; j++) {
            means[j] += arr_2d[i][j];
        }
    }
    // 個体数で割る
    for (j = 0; j < PRM1L_LEN; j++) {
        means[j] /= POPULATION;
    }
    // 各個体ベクトルから平均ベクトルを引いて偏差の二乗に書き換え
    // 重心を原点にするってことでいいのかな?
    for (i = 0; i < POPULATION; i++) {
        sqDistArray(arr_2d[i], means, arr_2d[i], PRM1L_LEN);
        // 各個体の平均ベクトルからの二乗距離を計算
        dists[i] = sumFloat(arr_2d[i], PRM1L_LEN);
        // 平方根を計算してユークリッド距離にする
        dists[i] = sqrtf(dists[i]);
    }
    // 平均ベクトルからの距離の平均を計算して表示
    // まじでややこしい
    printf("the average norm of deviation vector??: ");
    printFloat(aveFloat(dists, POPULATION));
}