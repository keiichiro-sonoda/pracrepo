from ctypes import *
import numpy as np
from matplotlib import pyplot as plt

exe2_win = cdll.LoadLibrary("exe2_win.so")
# 配列型定義
FloatArray64 = c_float * 64
FloatArray10 = c_float * 10
# 初期化関数
exe2_win.initPy()
# ただ確認するだけの関数
checkSprmFile = exe2_win.checkSprmFile
checkSprmFile.rectype = None
checkSprmFile.argtypes = (c_int32,)
#checkSprmFile(600)

getSprmFile = exe2_win.getSprmFilePy
getSprmFile.rectype = None
getSprmFile.argtypes = (c_int32, FloatArray64)

getTop10Ave = exe2_win.getTop10AvePy
getTop10Ave.rectype = None
getTop10Ave.argtypes = (c_int32, FloatArray10)

def getSprmFileWrap(n):
    f_arr_c = FloatArray64()
    getSprmFile(n, f_arr_c)
    return list(f_arr_c)

def getTop10AveWrap(n):
    f_arr_c = FloatArray10()
    getTop10Ave(n, f_arr_c)
    return

def multGraphSample():
    # x 全部同じやん
    x_1 = np.linspace(0, 1, 100)
    y_1 = x_1
    x_2 = np.linspace(0, 1, 100)
    y_2 = x_2 ** 2
    x_3 = np.linspace(0, 1, 100)
    y_3 = x_3 ** 3
    x_4 = np.linspace(0, 1, 100)
    y_4 = x_4 ** 4
    # ウィンドウ的な何か
    # ここでグラフのサイズ調整も
    fig = plt.figure(figsize=(12, 8))

    # 2 x 2 の枠にグラフを描画?
    # 左上
    ax_1 = fig.add_subplot(221)
    # 右上
    ax_2 = fig.add_subplot(222)
    # 左下
    ax_3 = fig.add_subplot(223)
    # 右下
    ax_4 = fig.add_subplot(224)

    ax_1.plot(x_1, y_1)
    ax_2.plot(x_2, y_2)
    ax_3.plot(x_3, y_3)
    ax_4.plot(x_4, y_4)
    plt.show()

    print("end?")

# なんとなく関数保存
def dataView01():
    x_min = 0
    x_max = 100
    y_min = -0.5
    y_max = 0.5
    x = []
    # 10 マス分のデータの配列を用意
    ys = [[] for i in range(10)]
    # 等価マスを省略
    essence = (0, 1, 2, 3, 9, 10, 11, 18, 19, 23)
    #print(ys)
    for i in range(x_min, x_max + 1):
        # x は範囲内の整数全て
        x.append(i)
        # i 世代のパラメータを取り出す(等価マスも含む64個)
        tprm = getSprmFileWrap(i)
        # 主要な10個の要素だけ取り出す
        for j, k in enumerate(essence):
            ys[j].append(tprm[k])
    
    # 使い慣れたいからオブジェクト指向にしよう
    fig = plt.figure(figsize=(12, 8))
    ax = fig.add_subplot(111)
    # 各マスの変移をプロット
    for i in range(10):
        lc = "#000000"
        if i == 0:
            lc = "b"
        elif i == 4:
            lc = "r"
        # ラベル付け
        ax.plot(x, ys[i], label="{:d}".format(i + 1), color=lc)
    plt.legend(loc="best")
    plt.show()

if __name__ == "__main__":
    #dataView01()
    getTop10AveWrap(100)