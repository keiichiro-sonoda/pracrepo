# 突然変異除いたバージョンのパラメータのグラフを作りたい
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

# 文字列を渡す
def getTop10AveFlexWrap(fnamer):
    f_arr_c = FloatArray10()
    # バイト型に直して与える
    getTop10AveFlex(fnamer.encode(), f_arr_c)
    return list(f_arr_c)

# グラフ用の色
LINE_COLORS = [
    "#00ff00", "#1f77b4", "#ff7f0e", "#9467bd", "#ff0000",
    "#8c564b", "#e377c2", "#7f7f7f", "#bcbd22", "#17becf"
]

# 平均値表示したい(ランダム対戦適応計算)
def dataView04():
    # ファイル名のフォーマット
    fname_format = "prm/sprm_vsrand{:03d}.bin"
    x_min = 0
    x_max = 100
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
    ax.set_xlabel("generation", fontsize=15)
    ax.set_ylabel("point", fontsize=15)
    plt.show()

# 平均値表示したい(課題4で発表したグラフの世代縮小バージョン?)
# ファイル名のフォーマットを引数で指定
def dataView05(fname_format):
    x_min = 0
    x_max = 100
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
    ax.set_xlabel("generation", fontsize=15)
    ax.set_ylabel("point", fontsize=15)
    plt.show()

if __name__ == "__main__":
    dataView05("prm//simple_prm{:03d}.bin")