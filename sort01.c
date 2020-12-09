#include <time.h>

#include "sort01.h"
#include "othello.h"

// for debugging
void sortTest(void) {
    srand(123u);
    srand((unsigned)time(NULL));
    int sample1[] = {11, 5, 6, 8, 1, 2, 10, 3, -5, 4, 2, 10, 9, 7, 20, 0, -2, -1};
    float sample2[] = {-100.0, -1.0, -0.5, -0.2, 93.8, 0.1, 0.3, 1.0, 2.5, -11.0, 4.0};
    double sample3[] = {1., .5, 1.0e-3, 0., 10., 3.829e+1, .25, 1.e-2, .5e+3, 3.e-3};
    int l1 = arrayLength(sample1);
    int l2 = arrayLength(sample2);
    int l3 = arrayLength(sample3);
    l3 = 50;
    int l4 = 49;
    int result[l4];
    int total_result[l3];
    zeros(total_result, l3);
    kugiri(l1);
    kugiri(l2);
    double sample4[l3];
    int numbers[l3];
    indices(numbers, l3);
    powArray(numbers, sample4, l3, 0.9);
    printFloatArrayExp(sample4, l3);
    for (int i = 0; i < 100000; i++) {
        rouletteDoubleMltDep(sample4, l3, result, l4);
        for (int j = 0; j < l4; j++) {
            total_result[result[j]]++;
        }
    }
    printDecimalArray(total_result, l3);
}

// insertion sort
void insertionSort(int *A, int n) {
    int i, j, key;
    for (j = 1; j < n; j++) {
        key = A[j];
        // A[j]をソート済みの列A[1..j-1]に挿入する.
        for (i = j - 1; i >= 0 && A[i] > key; i--)
            A[i + 1] = A[i];
        A[i + 1] = key;
    }
}

// 重複しないように指定された範囲の整数乱数を2つ取得
// randomly select 2 integers without duplication
void randIntDoubleDep(int pair[2], int min, int max) {
    pair[0] = randIntRange(min, max);
    // 最大値を1減らす (取り得る範囲を小さくする)
    pair[1] = randIntRange(min, max - 1);
    // 1回目の乱数以上なら1を足して調整
    if (pair[1] >= pair[0])
        pair[1]++;
}

// 重複しないように指定された数だけ範囲内のの整数乱数を取得
void randIntMltDep(int *rd_nums, int n, int min, int max) {
    int i, tmp_max;
    for (i = 0; i < n; i++) {
        // くり返す毎に範囲が狭まる
        tmp_max = max - i;
        // 取り得る整数が尽きた場合
        if (tmp_max < min) break;
        rd_nums[i] = randIntRange(min, tmp_max);
    }
    // 添字修復
    // 指定された数が上限を越えていても一応動くように?
    fixIndices(rd_nums, i);
}

// make an array of exponents
// to make the numbers positive
void expArray(const float *X, float *Y, int n) {
    for (int i = 0; i < n; i++)
        Y[i] = (float)exp(X[i]);
}

// make an array of exponents
// int -> float
// give the base of exponents
// the base is only support non-negative number
void expArrayIFFlex(const int *X, float *Y, int n, float base) {
    for (int i = 0; i < n; i++)
        Y[i] = powf(base, (float)X[i]);
}

// return the sum of an integer array
int sumInt(const int *A, int n) {
    int s = 0;
    addAll(A, n, s);
    return s;
}

// return the sum of a floating point array
float sumFloat(const float *A, int n) {
    float s = .0f;
    addAll(A, n, s);
    return s;
}

// double 型の配列の和を返す
double sumDouble(const double *A, int n) {
    double s = .0;
    addAll(A, n, s);
    return s;
}

// delete the element of the specified index
// the size of the array doesn't change
void delInt(int *A, int n, int index) {
    for (int i = index + 1; i < n; i++)
        A[i - 1] = A[i];
}

// delete the element of the specified index
// the size of the array doesn't change
void delFloat(float *A, int n, int index) {
    for (int i = index + 1; i < n; i++)
        A[i - 1] = A[i];
}

// roulette selection
// only supports non-negative integers
// return an index
int rouletteInt(const int *A, int n, int s) {
    // avoid dividing by 0
    if (s == 0) return 0;
    int r = rand() % s;
    int i;
    for (i = 0; i < n - 1; i++) {
        r -= A[i];
        if (r < 0)
            return i;
    }
    return i;
}

// roulette selection
// only supports non-negative floating point numbers
int rouletteFloat(const float *A, int n, float s) {
    float r = (float)rand() / RAND_MAX * s;
    int i;
    for (i = 0; i < n - 1; i++) {
        r -= A[i];
        if (r < 0)
            return i;
    }
    return i;
}

// double 型の配列でルーレット選択
// 値は非負であること前提
int rouletteDouble(const double *A, int n, double s) {
    // 戻り値兼イテレータ変数
    int i = 0;
    double rd, cum;
    // 0 から配列の合計値までの乱数
    rd = randDouble() * s;
    // 累積を格納する変数に先頭要素を代入 (初期化も兼ねて)
    cum = A[i++];
    do {
        // 累積が乱数を上回ったとき, ループを抜ける
        if (cum > rd) break;
        // 累積に加算 (ついでにインクリメント)
        cum += A[i++];
    }
    while (i < n);
    // 終了時の添字-1を返す
    return i - 1;
}

// 重複なし選択の際に起こる添字のズレを修正
// 後に選ばれた添字が先に選ばれた添字以上の場合, 後に選ばれた添字をインクリメントする
// 後ろから前に修正していくことが重要?
// fix the indices misalignment
void fixIndices(int *A, int n) {
    // 末尾の一つ前の添字から
    for (int i = n - 2; i >= 0; i--)
        for (int j = i + 1; j < n; j++)
            if (A[i] <= A[j])
                A[j]++;
}

// choose some elements with roulette
// don't allow duplication
// only supports non-negative integers
void rouletteIntMltDep(const int *A, int A_len, int *rslt, int rslt_len) {
    zeros(rslt, rslt_len);
    int now[A_len];
    copyArray(A, now, A_len);
    int now_len;
    int s = sumInt(A, A_len);
    for (int i = 0; i < rslt_len; i++) {
        now_len = A_len - i;
        //printDecimalArray(now, now_len);
        //printDecimal(s);
        rslt[i] = rouletteInt(now, now_len, s);
        s -= now[rslt[i]];
        // delete the selected element
        delInt(now, now_len, rslt[i]);
    }
    // fix the results
    fixIndices(rslt, rslt_len);
}

// choose some elements with roulette
// don't allow duplication
void rouletteFloatMltDep(const float *A, int A_len, int *rslt, int rslt_len) {
    // initialize
    zeros(rslt, rslt_len);
    float now[A_len];
    copyArray(A, now, A_len);
    int now_len;
    float s = sumFloat(A, A_len);
    for (int i = 0; i < rslt_len; i++) {
        now_len = A_len - i;
        rslt[i] = rouletteFloat(now, now_len, s);
        s -= now[rslt[i]];
        // delete the selected element
        delFloat(now, now_len, rslt[i]);
    }
    // fix the results
    fixIndices(rslt, rslt_len);
}

// ルーレット選択で重複なしで要素を選択
// 選ばれた要素は選択肢から排除
void rouletteDoubleMltDep(const double *A, int A_len, int *rslt, int rslt_len) {
    double s, cand[A_len];
    // 選択候補の配列を初期化
    copyArray(A, cand, A_len);
    s = sumDouble(A, A_len);
    // 選択数が要素数を越えていたら…要素数に合わせる
    // その場合選ばれる組み合わせは全ての要素で固定される
    if (A_len < rslt_len) {
        rslt_len = A_len;
    }
    for (int i = 0; i < rslt_len; i++) {
        // 候補の中からルーレット選択
        rslt[i] = rouletteDouble(cand, A_len, s);
        // 合計値から選ばれた値を引く
        s -= cand[rslt[i]];
        // 要素を排除 (左詰め)
        delElement(cand, A_len, rslt[i]);
        // 仮引数 A_len は候補の長さとして扱い, ループ毎にデクリメント
        A_len--;
    }
    // 左詰めした添字は元に戻す
    fixIndices(rslt, rslt_len);
}

// check if rouletteFloat workes as expected
void rouletteFloatTest(const float *A, int n) {
    float s = sumFloat(A, n);
    int loop = s * 10000;
    printDecimal(loop);
    int results[n];
    // initialize
    zeros(results, n);
    for (int i = 0; i < loop; i++)
        results[rouletteFloat(A, n, s)]++;
    printDecimalArray(results, n);
}

// 降順
// 基準は A[r] で固定される
int partitionDD(int *A, int *B, int p, int r) {
    int i, j;
    int x = A[r];
    i = p;
    for (j = p; j < r; j++) {
        if (A[j] > x) {
            exchangeD(int, A, B, i, j);
            i++;
        }
    }
    exchangeD(int, A, B, i, j);
    return i;
}

// 乱択版パーティション (昇順)
int randomizedPartition(int *A, int p, int r) {
    // マクロ内で乱数を作成すると, 展開されたときに二度実行されてしまう
    int i = randIntRange(p, r);
    exchange(int, A, i, r);
    int x = A[r];
    i = p;
    for (int j = p; j < r; j++) {
        if (A[j] < x) {
            exchange(int, A, i, j);
            i++;
        }
    }
    exchange(int, A, i, r);
    return i;
}

// randomized partition
// A: array to compare
// B: sorted by A
int randomizedPartitionDD(int *A, int *B, int p, int r) {
    // 分割の基準となる値はランダムに決定する
    int i = randIntRange(p, r);
    // randomized!
    exchangeD(int, A, B, i, r);
    int x = A[r];
    i = p;
    for (int j = p; j < r; j++) {
        if (A[j] > x) {
            exchangeD(int, A, B, i, j);
            i++;
        }
    }
    exchangeD(int, A, B, i, r);
    return i;
}

// descending order
// also rearrange the indices
void quicksortDD(int *A, int *B, int p, int r) {
    if (p < r) {
        int q = partitionDD(A, B, p, r);
        quicksortDD(A, B, p, q - 1);
        quicksortDD(A, B, q + 1, r);
    }
}

// 乱択版クイックソート (昇順)
void randomizedQuicksort(int *A, int p, int r) {
    if (p < r) {
        int q = randomizedPartition(A, p, r);
        randomizedQuicksort(A, p, q - 1);
        randomizedQuicksort(A, q + 1, r);
    }
}

// randomized quicksort
void randomizedQuicksortDD(int *A, int *B, int p, int r) {
    if (p < r) {
        int q = randomizedPartitionDD(A, B, p, r);
        randomizedQuicksortDD(A, B, p, q - 1);
        randomizedQuicksortDD(A, B, q + 1, r);
    }
}