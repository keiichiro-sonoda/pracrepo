# グラフ作成ファイル
from ctypes import *
import numpy as np
from matplotlib import pyplot as plt

# 共有ライブラリ読み込み(同じディレクトリで実行)
exe2_win = cdll.LoadLibrary("exe2_win.so")
# 配列型定義
FloatArray10 = c_float * 10
# 初期化関数
exe2_win.initPy()

getTop10AveFlex = exe2_win.getTop10AveFlexPy
getTop10AveFlex.rectype = None
getTop10AveFlex.argtypes = (c_char_p, FloatArray10)

getFamilyMeans = exe2_win.getFamilyMeansPy
getFamilyMeans.rectype = c_int32
getFamilyMeans.argtypes = (c_char_p, FloatArray10, c_int32)

# 標準偏差の配列を計算する関数を共有ライブラリから取得
getTop10SDFlex = exe2_win.getTop10SDFlexPy
getTop10SDFlex.rectype = None
getTop10SDFlex.argtypes = (c_char_p, FloatArray10)

getFamilySD = exe2_win.getFamilySDPy
getFamilySD.rectype = c_int32
getFamilySD.argtypes = (c_char_p, FloatArray10, c_int32)

# 各種ラッパー関数
# 文字列を渡す
def getTop10AveFlexWrap(fnamer):
    f_arr_c = FloatArray10()
    # バイト型に直して与える
    getTop10AveFlex(fnamer.encode(), f_arr_c)
    return list(f_arr_c)

def getFamilyMeansWrap(fnamer, n):
    f_arr_c = FloatArray10()
    flag = getFamilyMeans(fnamer.encode(), f_arr_c, n)
    # 読み込み失敗
    if flag < 0:
        return None
    return list(f_arr_c)

def getTop10SDFlexWrap(fnamer):
    f_arr_c = FloatArray10()
    getTop10SDFlex(fnamer.encode(), f_arr_c)
    return list(f_arr_c)

def getFamilySDWrap(fnamer, n):
    f_arr_c = FloatArray10()
    flag = getFamilySD(fnamer.encode(), f_arr_c, n)
    if flag < 0:
        return []
    return list(f_arr_c)

# グラフ用の色
LINE_COLORS = [
    "#00ff00", "#1f77b4", "#ff7f0e", "#9467bd", "#ff0000",
    "#8c564b", "#e377c2", "#7f7f7f", "#bcbd22", "#17becf"
]

# 平均値表示したい(課題4で発表したグラフの世代縮小バージョン?)
# ファイル名のフォーマットを引数で指定してより柔軟にする
# 世代幅も指定
def dataView05(fname_format, x_min, x_max):
    x = []
    # 10 マス分のデータの配列を用意
    ys = [[] for i in range(10)]
    for i in range(x_min, x_max + 1):
        # x は範囲内の整数全て
        x.append(i)
        # i 世代のトップ10の平均値を取り出す
        tprm = getTop10AveFlexWrap(fname_format.format(i))
        # それぞれのマスの評価値に代入!
        for j in range(10):
            ys[j].append(tprm[j])
    
    # 使い慣れたいからオブジェクト指向にしよう
    fig = plt.figure(figsize=(8, 5))
    ax = fig.add_subplot(
        111,
        xlabel="generation",
        ylabel="point",
    )
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
    # ラベル指定
    ax.set_xlabel("generation", fontsize=15)
    ax.set_ylabel("point", fontsize=15)
    # 横幅指定（世代幅）
    ax.set_xticks(np.linspace(x_min, x_max, 11))
    # 縦幅指定（固定）
    ax.set_yticks(np.linspace(-0.6, 0.6, 7))
    plt.show()

# 標準偏差表示したい
# 平均値のときと呼び出す関数が変わるだけ
def dataView06(fname_format, x_min, x_max):
    x = []
    # 10 マス分のデータの配列を用意
    ys = [[] for i in range(10)]
    for i in range(x_min, x_max + 1):
        # x は範囲内の整数全て
        x.append(i)
        # i 世代のトップ10の標準偏差を取り出す
        tprm = getTop10SDFlexWrap(fname_format.format(i))
        # それぞれのマスの評価値に代入!
        for j in range(10):
            ys[j].append(tprm[j])
    
    # 使い慣れたいからオブジェクト指向にしよう
    fig = plt.figure(figsize=(8, 5))
    ax = fig.add_subplot(
        111,
        xlabel="generation",
        ylabel="point",
    )
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
    # 横幅指定（世代幅）
    ax.set_xticks(np.linspace(x_min, x_max, 11))
    # 縦幅指定（固定）
    ax.set_yticks(np.linspace(-0.0, 0.40, 5))
    plt.show()

# 平均値表示(各世代全個体)
# ファイル名のフォーマットと個体数を渡す
# 世代幅も指定
def makeMeansGraph(fname_format, population, x_min, x_max):
    x = []
    # 10 マス分のデータの配列を用意
    ys = [[] for i in range(10)]
    for i in range(x_min, x_max + 1):
        # i 世代全個体の平均値を取り出す
        tprm = getFamilyMeansWrap(fname_format.format(i), population)
        # 読み込みエラー
        if tprm == None:
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
    ax = fig.add_subplot(
        111,
        xlabel="generation",
        ylabel="point",
    )
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
    # ラベル指定
    ax.set_xlabel("generation", fontsize=15)
    ax.set_ylabel("point", fontsize=15)
    # 横幅指定（読み込みに成功したデータだけ）
    ax.set_xticks(np.linspace(x[0], x[-1], 11))
    # 縦幅指定（固定）
    ax.set_yticks(np.linspace(-0.6, 0.6, 7))
    plt.show()

# 標準偏差表示
def makeSDGraph(fname_format, population, x_min, x_max):
    x = []
    # 10 マス分のデータの配列を用意
    ys = [[] for i in range(10)]
    for i in range(x_min, x_max + 1):
        # x は範囲内の整数全て
        x.append(i)
        # i 世代全個体の標準偏差を取り出す
        tprm = getFamilySDWrap(fname_format.format(i), population)
        # それぞれのマスの評価値に代入!
        for j in range(10):
            ys[j].append(tprm[j])
    
    # 使い慣れたいからオブジェクト指向にしよう
    fig = plt.figure(figsize=(8, 5))
    ax = fig.add_subplot(
        111,
        xlabel="generation",
        ylabel="point",
    )
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
    # 横幅指定（世代幅）
    ax.set_xticks(np.linspace(x_min, x_max, 11))
    # 縦幅指定（固定）
    ax.set_yticks(np.linspace(-0.0, 0.40, 5))
    plt.show()

if __name__ == "__main__":
    # 最初
    #dataView05("prm//simple_prm{:03d}.bin", 0, 100)
    # 突然変異無し
    #dataView05("prm//sprm_not_mutate{:03d}.bin", 0, 20)
    # 角固定
    #dataView05("prm//sprm_corner0.5_{:03d}.bin", 0, 100)
    # ランダム対戦
    #dataView05("prm//sprm_vsrand{:03d}.bin", 0, 100)
    # 角固定(-0.5)
    #dataView05("prm//sprm_corner0.5neg_{:03d}.bin", 0, 100)
    # 指し手をルーレット選択
    #dataView05("prm//sprm_roulette{:03d}.bin", 0, 100)
    #dataView06("prm//sprm_roulette{:03d}.bin", 0, 100)
    # 指し手ルーレット, 勝ち点で直接ルーレット選択
    #dataView05("prm//sprm_rltrlt{:03d}.bin", 0, 100)
    #dataView06("prm//sprm_rltrlt{:03d}.bin", 0, 100)
    # 指し手ルーレット, 最低点を引いた点でルーレット選択
    #dataView05("prm//sprm_rr_submin{:03d}.bin", 0, 100)
    #dataView06("prm//sprm_rr_submin{:03d}.bin", 0, 100)
    # 指し手ルーレット, 100-順位を確率(相対)にしたランキング選択
    #dataView05("prm//sprm_rrank{:03d}.bin", 0, 100)
    #dataView06("prm//sprm_rrank{:03d}.bin", 0, 100)
    # 指し手ルーレット, 0.9 ^ 順位を確率(相対)にしたランキング選択
    #dataView05("prm//sprm_rrank_exp//sprm_rrank_exp{:03d}.bin", 0, 100)
    #dataView06("prm//sprm_rrank_exp//sprm_rrank_exp{:03d}.bin", 0, 100)
    # 適応度評価を行なわない
    #dataView05("prm/sprm_nofit/sprm_nofit{:03d}.bin", 0, 100)
    #dataView06("prm/sprm_nofit/sprm_nofit{:03d}.bin", 0, 100)
    #getFamilyMeansWrap("prm/sprm050_06_rlt_uni_rd005/sprm050_06_rlt_uni_rd005_g100.bin", 50)
    makeMeansGraph("prm/sprm050_06_rlt_uni_rd005/sprm050_06_rlt_uni_rd005_g{:03d}.bin", 50, 0, 100)
    #makeSDGraph("prm/sprm050_06_rlt_uni_rd005/sprm050_06_rlt_uni_rd005_g{:03d}.bin", 50, 0, 100)