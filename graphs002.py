# グラフ作成ファイル
from ctypes import *
import numpy as np
import math
from matplotlib import pyplot as plt

# 共有ライブラリ読み込み(同じディレクトリで実行)
exe2_win = cdll.LoadLibrary("exe2_win.so")
# 配列型定義
FloatArray10 = c_float * 10
FloatArray64 = c_float * 64
# 初期化関数
exe2_win.initPy()

# 全個体の平均値
getFamilyMeans = exe2_win.getFamilyMeansPy
getFamilyMeans.rectype = c_int32
getFamilyMeans.argtypes = (c_char_p, FloatArray10, c_int32)

# 全個体の標準偏差
getFamilySD = exe2_win.getFamilySDPy
getFamilySD.rectype = c_int32
getFamilySD.argtypes = (c_char_p, FloatArray10, c_int32)

# あるファイルの先頭の要素
getTopSprm = exe2_win.getTopSprmPy
getTopSprm.rectype = c_int32
getTopSprm.argtypes = (c_char_p, FloatArray64)

# 各種ラッパー関数
# n は個体数を指定
# オーバーに注意
def getFamilyMeansWrap(fnamer, n):
    f_arr_c = FloatArray10()
    flag = getFamilyMeans(fnamer.encode(), f_arr_c, n)
    # 読み込み失敗
    if flag < 0:
        return []
    return list(f_arr_c)

def getFamilySDWrap(fnamer, n):
    f_arr_c = FloatArray10()
    flag = getFamilySD(fnamer.encode(), f_arr_c, n)
    if flag < 0:
        return []
    return list(f_arr_c)

def getTopSprmWrap(fnamer):
    f_arr_c = FloatArray64()
    if getTopSprm(fnamer.encode(), f_arr_c) < 0:
        return []
    return list(f_arr_c)

# グラフ用の色
LINE_COLORS = [
    "#00ff00", "#1f77b4", "#ff7f0e", "#9467bd", "#ff0000",
    "#8c564b", "#e377c2", "#7f7f7f", "#bcbd22", "#17becf"
]

# 平均値のグラフを作成
def makeMeansGraph(ax, x, ys):
    step = 0.2
    # 各マスの変移をプロット
    for i in range(10):
        lw = 1
        lc = LINE_COLORS[i]
        if i in [0, 4]:
            lw = 4
        # ラベル付け
        ax.plot(x, ys[i],
            label="{:d}".format(i + 1),
            color=lc,
            linewidth=lw
        )
    #plt.legend(loc="best")
    # 凡例調節
    ax.legend(
        bbox_to_anchor=(1.01, 1),
        loc='upper left',
        borderaxespad=0,
        fontsize=10
    )
    #ax.grid()
    # ラベル指定
    ax.set_xlabel("generation", fontsize=15)
    ax.set_ylabel("means", fontsize=15)
    # 横幅指定（読み込みに成功したデータだけ）
    ax.set_xticks(np.linspace(x[0], x[-1], 11))
    # 最大値と最小値を取得
    y_max = math.ceil(max(max(i) for i in ys) / step) * step
    y_min = math.floor(min(min(i) for i in ys) / step) * step
    print(y_max, y_min)
    # 縦幅指定（固定）
    ax.set_yticks(np.arange(y_min, y_max + step / 2, step))

# 平均値表示(各世代全個体)
# ファイル名のフォーマットと個体数を渡す
# 世代幅も指定
def viewMeansGraph(fname_format, population, x_min, x_max):
    x = []
    # 10 マス分のデータの配列を用意
    ys = [[] for i in range(10)]
    for i in range(x_min, x_max + 1):
        # i 世代全個体の平均値を取り出す
        tprm = getFamilyMeansWrap(fname_format.format(i), population)
        # 読み込みエラー
        if not tprm:
            continue
        # 読み込めたらデータを追加
        # x は範囲内の整数全て
        x.append(i)
        # それぞれのマスの評価値に代入!
        for j in range(10):
            ys[j].append(tprm[j])

    # 読み込めたデータが無ければ終了
    if not x:
        return
    
    fig = plt.figure(figsize=(8, 5))
    ax = fig.add_subplot(111)
    makeMeansGraph(ax, x, ys)
    plt.show()

# 標準偏差のグラフを作成
def makeSDGraph(ax, x, ys):
    # 各マスの変移をプロット
    for i in range(10):
        lw = 1
        lc = LINE_COLORS[i]
        # 注目マス
        # 標準偏差はいらないかも
        #if i in [0, 4]:
        #    lw = 4
        # ラベル付け
        ax.plot(x, ys[i],
            label="{:d}".format(i + 1),
            color=lc,
            linewidth=lw
        )
    #plt.legend(loc="best")
    # 凡例調節
    ax.legend(
        bbox_to_anchor=(1.01, 1),
        loc='upper left',
        borderaxespad=0,
        fontsize=10
    )
    # ラベル指定
    ax.set_xlabel("generation", fontsize=15)
    ax.set_ylabel("standard deviation", fontsize=15)
    # 横幅指定（読み込めたデータだけ）
    ax.set_xticks(np.linspace(x[0], x[-1], 11))
    # 縦幅指定（固定）
    ax.set_yticks(np.linspace(-0.0, 0.40, 5))

# 標準偏差表示
def viewSDGraph(fname_format, population, x_min, x_max):
    x = []
    # 10 マス分のデータの配列を用意
    ys = [[] for i in range(10)]
    for i in range(x_min, x_max + 1):
        # i 世代全個体の標準偏差を取り出す
        tprm = getFamilySDWrap(fname_format.format(i), population)
        # 空リストがの場合（読み込み失敗）
        if not tprm:
            continue
        # x は読み込めた整数全て
        x.append(i)
        # それぞれのマスの評価値に代入!
        for j in range(10):
            ys[j].append(tprm[j])
    
    # 読み込めたデータがひとつもない
    if not x:
        return
    
    # 使い慣れたいからオブジェクト指向にしよう
    fig = plt.figure(figsize=(8, 5))
    ax = fig.add_subplot(111)
    makeSDGraph(ax, x, ys)
    plt.show()

# 2つのグラフを同時描画したい
def viewStatGraphs(fname_format, population, g_min, g_max):
    g = []
    # 10 マス分のデータの配列を用意
    means = [[] for i in range(10)]
    SD = [[] for i in range(10)]
    for i in range(g_min, g_max + 1):
        fname = fname_format.format(i)
        # i 世代全個体の平均値
        tmp1 = getFamilyMeansWrap(fname, population)
        # 空リストがの場合（読み込み失敗）
        # 平均値が読み込める=標準偏差も読み込めると考える
        if not tmp1:
            continue
        # 標準偏差も読み込む
        tmp2 = getFamilySDWrap(fname, population)
        # x は読み込めた整数全て
        g.append(i)
        # それぞれのマスの評価値に代入!
        for j in range(10):
            means[j].append(tmp1[j])
            SD[j].append(tmp2[j])
    
    # 読み込めたデータがひとつもない
    if not g:
        return
    
    # 使い慣れたいからオブジェクト指向にしよう
    fig = plt.figure(figsize=(8, 10))
    ax1 = fig.add_subplot(211)
    ax2 = fig.add_subplot(212)
    makeMeansGraph(ax1, g, means)
    makeSDGraph(ax2, g, SD)
    plt.show()

# 関数テスト
def funcTest(fname_format, generation):
    fname = fname_format.format(generation)
    l = getTopSprmWrap(fname)
    z = np.zeros((8, 8))
    for i in range(8):
        for j in range(8):
            z[i, j] = l[i + j * 8]
    plt.imshow(z, cmap='viridis')
    plt.colorbar()
    plt.show()

# ファイルフォーマットのリスト
FILE_FORMATS = [# 00. から10. は選ばれた10個体のみファイルに保存
                # 00. 最初 (指し手固定)
                "prm//simple_prm//simple_prm{:03d}.bin",
                # 01. 突然変異無し (20世代まで)
                "prm//sprm_not_mutate//sprm_not_mutate{:03d}.bin",
                # 02. 角固定
                "prm//sprm_corner0.5//sprm_corner0.5_{:03d}.bin",
                # 03. ランダム対戦
                "prm//sprm_vsrand//sprm_vsrand{:03d}.bin",
                # 04. 角固定-0.5
                "prm//sprm_corner0.5neg//sprm_corner0.5neg_{:03d}.bin",
                # 以下, 指し手をルーレット選択
                # 05. 最初
                "prm//sprm_roulette//sprm_roulette{:03d}.bin",
                # 06.  勝ち点で直接ルーレット
                "prm//sprm_rltrlt//sprm_rltrlt{:03d}.bin",
                # 07. 最低点を引いて+1した値でルーレット
                "prm//sprm_rr_submin//sprm_rr_submin{:03d}.bin",
                # 08. 100引く順位（線形）に比例した確率でランキング選択
                "prm//sprm_rrank//sprm_rrank{:03d}.bin",
                # 09. 0.9 ^ 順位（指数）に比例した確率でランキング選択
                "prm//sprm_rrank_exp//sprm_rrank_exp{:03d}.bin",
                # 10. 適応度評価を行わない（比較用）
                "prm//sprm_nofit//sprm_nofit{:03d}.bin",
                # 以下, 全個体ファイルに保存
                # 11. 個体数50, エリート6, 非独立ルーレット選択, 一様交叉, ランダム突然変異5%
                "prm//sprm050_06_rlt_uni_rd005//sprm050_06_rlt_uni_rd005_g{:03d}.bin",
                # 12. 個体数100, エリート10, 非独立ランダム選択, 平均と一様一回ずつ, 一様のみランダム突然変異5%
                "prm//sprm100_10_rd_au_rd005//sprm100_10_rd_au_rd005_g{:03d}.bin",
                # 13. 個体数50, エリート6, 非独立ランダム選択, 平均と一様一回ずつ, 一様のみランダム突然変異5%
                "prm//sprm050_06_rd_au_rd005//sprm050_06_rd_au_rd005_g{:03d}.bin",
                # 14. 個体数50, エリート6, 非独立ルーレット選択, 平均と一様一回ずつ, 一様のみランダム突然変異5%
                "prm//sprm050_06_rlt_au_rd005//sprm050_06_rlt_au_rd005_g{:03d}.bin",
                # 15. 個体数50, エリート6, 非独立ランダム選択, 一様交叉, ランダム突然変異5%
                "prm//sprm050_06_rd_uni_rd005//sprm050_06_rd_uni_rd005_g{:03d}.bin",
                # 16. 個体数50, エリート6, 非独立ルーレット選択, 一点交叉, ランダム突然変異5%
                "prm//sprm050_06_rlt_1p_rd005//sprm050_06_rlt_1p_rd005_g{:03d}.bin",
                # 17. 指し手固定再び, 個体数50, エリート6, 非独立ルーレット選択, 一様交叉, ランダム突然変異5%
                "prm//sprmdef050_06_rlt_uni_rd005//sprmdef050_06_rlt_uni_rd005_g{:03d}.bin",
                # 18. 個体数50, エリート6, 非独立ルーレット選択, 一点交叉, シフト突然変異5%
                "prm//sprm050_06_rlt_uni_sft005//sprm050_06_rlt_uni_sft005_g{:03d}.bin"]

if __name__ == "__main__":
    ind = 18
    viewStatGraphs(FILE_FORMATS[ind], 50, 0, 100)
    #viewMeansGraph(FILE_FORMATS[ind], 50, 0, 100)
    #funcTest(FILE_FORMATS[ind], 100)