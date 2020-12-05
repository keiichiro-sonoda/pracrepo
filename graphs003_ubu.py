# グラフ作成ファイル
# ubuntu で使うために分岐した
import json
import re
import os
from ctypes import *
import numpy as np
import math
from matplotlib import pyplot as plt

# シード値
SEED = 123

# グラフ描画のみ
VIEW_ONLY = True

# 共有ライブラリ読み込み(カレントディレクトリを想定)
# ubuntu用であることに注意
share02_ubu = cdll.LoadLibrary(".//share02_ubu.so")
# 配列型定義
FloatArray10 = c_float * 10
FloatArray64 = c_float * 64
IntArray3 = c_int32 * 3

# 初期化関数を実行 (バグの温床)
share02_ubu.initPy()

# 全個体の平均値
# 第四引数に圧縮フラグを設定
getFamilyMeans = share02_ubu.getFamilyMeansPy
getFamilyMeans.restype = c_int32
getFamilyMeans.argtypes = (c_char_p, FloatArray10, c_int32, c_int32)

# 全個体の標準偏差
# 第四引数に圧縮フラグを設定
getFamilySD = share02_ubu.getFamilySDPy
getFamilySD.restype = c_int32
getFamilySD.argtypes = (c_char_p, FloatArray10, c_int32, c_int32)

# あるファイルの先頭の要素
getTopSprm = share02_ubu.getTopSprmPy
getTopSprm.restype = c_int32
getTopSprm.argtypes = (c_char_p, FloatArray64)

# あるファイルの先頭要素をランダムAIと対戦させ, その結果を取得
getTopSprmGameRsltVSRand = share02_ubu.getTopSprmGameRsltVSRandPy
getTopSprmGameRsltVSRand.restype = c_int32
getTopSprmGameRsltVSRand.argtypes = (c_char_p, c_int32, c_int32, c_int32, c_int32, IntArray3)

# 各種ラッパー関数
# n は個体数を指定 (過小はいいがオーバーに注意)
# 圧縮フラグを追加
def getFamilyMeansWrap(fnamer, n, compressed):
    f_arr_c = FloatArray10()
    flag = getFamilyMeans(fnamer.encode(), f_arr_c, n, compressed)
    # 読み込み失敗
    if flag < 0:
        return []
    return list(f_arr_c)

# 圧縮フラグを追加
def getFamilySDWrap(fnamer, n, compressed):
    f_arr_c = FloatArray10()
    flag = getFamilySD(fnamer.encode(), f_arr_c, n, compressed)
    if flag < 0:
        return []
    return list(f_arr_c)

def getTopSprmWrap(fnamer):
    f_arr_c = FloatArray64()
    if getTopSprm(fnamer.encode(), f_arr_c) < 0:
        return []
    return list(f_arr_c)

# あるファイルの先頭要素とランダムAIとの試合結果を取得
# [勝ち数, 引き分け数, 負け数] の順のリストを返す
# エラーなら [0, 0, 0]
# 指し手決定関数も指定するように変更
# 0: 固定, 1: ルーレット
def getTopSprmGameRsltVSRandWrap(fnamer, color, loc_pop, decNxt_id, game_num):
    # 戻り値保存用
    i_arr_c = IntArray3()
    if getTopSprmGameRsltVSRand(fnamer.encode(), color, loc_pop, game_num, decNxt_id, i_arr_c) < 0:
        return [0, 0, 0]
    return list(i_arr_c)

# グラフ用の色
LINE_COLORS = [
    "#00ff00", "#1f77b4", "#ff7f0e", "#9467bd", "#ff0000",
    "#8c564b", "#e377c2", "#7f7f7f", "#bcbd22", "#17becf"
]

# グラフを保存するファイル名を決定する関数
# グラフの種類 (平均値や標準偏差), 描画する世代の範囲を与える
def makeJpegFileName(fname_format, name, g_min, g_max):
    m = re.match(r"prm(//.*)//", fname_format)
    if not m:
        print("一致するパターンがありません")
        return ""
    path = "//home//sonoda//Pictures//Graphs" + m.groups()[0]
    options = "_" + name + "_g{1:03d}-{0:03d}".format(g_max, g_min) + ".jpg"
    path += options
    print(path)
    return path

# 平均値のグラフを作成
def makeMeansGraph(ax, x, ys):
    # 縦軸の値の間隔
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
    # 縦幅指定 (間隔は固定)
    ax.set_yticks(np.arange(y_min, y_max + step / 2, step))

# 平均値表示(各世代全個体)
# ファイル名のフォーマットと個体数を渡す
# 世代幅も指定
# ついでに画像保存
# 圧縮フラグを追加
def viewMeansGraph(fname_format, population, x_min, x_max, compressed):
    x = []
    # 10 マス分のデータの配列を用意
    ys = [[] for i in range(10)]
    for i in range(x_min, x_max + 1):
        # i 世代全個体の平均値を取り出す
        tprm = getFamilyMeansWrap(fname_format.format(i), population, compressed)
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
    path = makeJpegFileName(fname_format, "means{:03d}".format(population), int(x[0]), int(x[-1]))
    if path and not VIEW_ONLY:
    # 出力画像の周囲の境界ボックス?を消す
        fig.savefig(path, bbox_inches="tight")
        print("saved!!")

# 標準偏差のグラフを作成
def makeSDGraph(ax, x, ys):
    # 各マスの変移をプロット
    for i in range(10):
        lw = 1
        lc = LINE_COLORS[i]
        # 注目マス (必要に応じて変更)
        if i == -1:
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
    ax.set_ylabel("standard deviation", fontsize=15)
    # 横幅指定（読み込めたデータだけ）
    ax.set_xticks(np.linspace(x[0], x[-1], 11))
    # 縦幅指定（固定）
    ax.set_yticks(np.linspace(-0.0, 0.40, 5))

# 標準偏差表示, グラフ画像作成
# 圧縮フラグを追加
def viewSDGraph(fname_format, population, x_min, x_max, compressed):
    x = []
    # 10 マス分のデータの配列を用意
    ys = [[] for i in range(10)]
    for i in range(x_min, x_max + 1):
        # i 世代全個体の標準偏差を取り出す
        tprm = getFamilySDWrap(fname_format.format(i), population, compressed)
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
    # フォーマットやグラフの範囲に合わせたパスを作成
    path = makeJpegFileName(fname_format, "SD{:03d}".format(population), int(x[0]), int(x[-1]))
    # 書き込み
    if path and not VIEW_ONLY:
        fig.savefig(path, bbox_inches="tight")
        print("saved!!")

# 2つのグラフを同時描画したい
# 圧縮フラグ追加
def viewStatGraphs(fname_format, population, g_min, g_max, compressed):
    g = []
    # 10 マス分のデータの配列を用意
    means = [[] for i in range(10)]
    SD = [[] for i in range(10)]
    for i in range(g_min, g_max + 1):
        fname = fname_format.format(i)
        # i 世代全個体の平均値
        tmp1 = getFamilyMeansWrap(fname, population, compressed)
        # 空リストがの場合（読み込み失敗）
        # 平均値が読み込める=標準偏差も読み込めると考える
        if not tmp1:
            continue
        # 標準偏差も読み込む
        tmp2 = getFamilySDWrap(fname, population, compressed)
        # x は読み込めた整数全て
        g.append(i)
        # それぞれのマスの評価値に代入!
        for j in range(10):
            means[j].append(tmp1[j])
            SD[j].append(tmp2[j])
    
    # 読み込めたデータがひとつもない
    if not g:
        return
    
    # グラフ作成
    fig = plt.figure(figsize=(8, 5))
    ax1 = fig.add_subplot(211)
    ax2 = fig.add_subplot(212)
    #fig.subplots_adjust(bottom=0.2, left=0.2, top=0.8, right=0.8)
    makeMeansGraph(ax1, g, means)
    makeSDGraph(ax2, g, SD)
    plt.show()

# 関数テスト
# パラメータの画像表示
def imgTest(fname_format, generation):
    fname = fname_format.format(generation)
    l = getTopSprmWrap(fname)
    z = np.zeros((8, 8))
    for i in range(8):
        for j in range(8):
            z[i, j] = l[i + j * 8]
    plt.imshow(z, cmap='viridis')
    plt.colorbar()
    plt.show()

# パラメータが保存されているファイルのフォーマットをjsonファイルのフォーマットに変更
# 指し手決定関数のidも与える
def makeJsonFileName(fname_format, decNxt_id):
    # json ファイルのフォーマットに使う部分を取得
    m = re.match(r"(prm//.*)//", fname_format)
    if not m:
        print("一致するパターンがありません")
        return ""
    # マッチオブジェクトから文字列に変換
    json_fname = m.groups()[0]
    if decNxt_id == 0:
        json_fname += "_best"
    else:
        json_fname += "_rlt"
    # 先頭の "prm" のみ "json" に書き換え (ディレクトリ変更)
    # 文字列の最後に "_wc.json" と付け加える (wc は win count の意)
    json_fname = json_fname.replace("prm", "json", 1) + "_wc.json"
    print(json_fname)
    return json_fname

# 各世代の代表者がランダムAIと対戦した結果の辞書を作ってjson形式で保存したい
# 世代番号をキーとし, 値は結果の辞書とする (白と黒それぞれの対戦結果)
def makeWinCountFile(fname_format, loc_pop, decNxt_id, game_num, g_min, g_max):
    # シードは直前に設定することにする
    # ランダムAIとの対戦なので一応再現性を確保
    share02_ubu.setSeedPy(SEED)
    # 書き込みファイル名作成
    json_fname = makeJsonFileName(fname_format, decNxt_id)
    if os.path.exists(json_fname):
        res = input(json_fname + "は存在します. 書き換えますか? (yes\\n)")
        if res != "yes":
            return
    # 保存するディクショナリ初期化
    wcd = {}
    # 指定した世代幅くり返し
    for i in range(g_min, g_max + 1):
        tmpd = {}
        # 読み込みファイル名
        fname = fname_format.format(i)
        # 失敗したら[0, 0, 0]が帰ってくる
        rslt = getTopSprmGameRsltVSRandWrap(fname, 1, loc_pop, decNxt_id, game_num)
        tmpd["black"] = rslt
        rslt = getTopSprmGameRsltVSRandWrap(fname, 2, loc_pop, decNxt_id, game_num)
        tmpd["white"] = rslt
        wcd[i] = tmpd
        print(wcd)
    f = open(json_fname, "w")
    json.dump(wcd, f)
    f.close()

# ランダムAIと対戦したときの勝率のグラフを作りたい
# 白黒は試合数が同じものとし, 平等に扱う
# とりあえず引き分けは考慮せず勝ちだけ考えよう
# 全ての世代で試合数は等しいと仮定 (違ったら同じグラフにするのおかしくね?)
def viewWinRateGraph(fname_format, decNxt_id):
    json_fname = makeJsonFileName(fname_format, decNxt_id)
    # ファイルの存在確認
    if not os.path.exists(json_fname):
        print(json_fname, "doesn't exist.")
        return
    f = open(json_fname, "r")
    wcd = json.load(f)
    f.close()
    # (世代番号, 勝率) のタプルのリスト
    wrl = []
    for k, v in wcd.items():
        # 試合数計算 (全て等しいと思うが毎回行う)
        game_num = sum(v["black"]) + sum(v["white"])
        # 勝率計算
        wr = (v["black"][0] + v["white"][0]) / game_num
        # キーはintにしてタプル化
        wrl.append((int(k), wr))
    # 辞書に順番という概念がないため一応ソート
    wrl.sort()
    # numpy配列に変換
    wra = np.array(wrl)
    # 世代数と勝率を別々の配列に分ける
    x = wra[:, 0]
    y = wra[:, 1]
    # ソートされているので最小値と最大値を取得
    x_min = int(x[0])
    x_max = int(x[-1])
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.plot(x, y)
    # ラベル指定
    ax.set_xlabel("generation", fontsize=15)
    ax.set_ylabel("win rate", fontsize=15)
    # 横幅指定（読み込めたデータだけ）
    ax.set_xticks(np.linspace(x_min, x_max, 11))
    # 縦幅指定（固定）
    ax.set_yticks(np.linspace(0.0, 1.0, 11))
    ax.grid()
    # game_num は最後に計算したものを使う
    path = makeJpegFileName(fname_format, "wr{:04d}".format(game_num), x_min, x_max)
    fig.savefig(path, bbox_inches="tight")

# フォーマットにシードも追加
# genetic02 のマクロ名と同じ
def formatPlusSeed(fname_format, seed):
    # .bin とそれ以外を区別
    m = re.match("(.+)(\.bin)", fname_format)
    if not m:
        print("一致するパターンがありません")
        return ""
    mg = m.groups()
    new = mg[0] + "_s{0:03d}".format(seed) + mg[1]
    return new

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
                # 17. 指し手固定, 個体数50, エリート6, 非独立ルーレット選択, 一様交叉, ランダム突然変異5%
                "prm//sprmdef050_06_rlt_uni_rd005//sprmdef050_06_rlt_uni_rd005_g{:03d}.bin",
                # 18. 個体数50, エリート6, 非独立ルーレット選択, 一様交叉, 制限無シフト突然変異5%
                "prm//sprm050_06_rlt_uni_sft005//sprm050_06_rlt_uni_sft005_g{:03d}.bin",
                # 19. 個体数50, エリート6, 非独立ルーレット選択, 一様交叉, 制限有シフト突然変異5%
                "prm//sprm050_06_rlt_uni_sftl005//sprm050_06_rlt_uni_sftl005_g{:03d}.bin",
                # 20. 個体数50, エリート6, 非独立ルーレット選択, 一様交叉, ランダム突変5%のあとに制限有シフト突変5%
                "prm//sprm050_06_rlt_uni_rdsftl005//sprm050_06_rlt_uni_rdsftl005_g{:03d}.bin",
                # 21. 指し手ルーレット, 個体数50, エリート6, 非独立ルーレット選択, 一様交叉, ランダム突変5%のあとに制限無シフト突変5%
                "prm//sprm050_06_rlt_uni_rdsft005//sprm050_06_rlt_uni_rdsft005_g{:03d}.bin",
                # 22. 指し手固定, 個体数50, エリート6, 非独立ランダム選択, 一様交叉, ランダム突変5%のあとに制限無シフト突変5%
                "prm//sprmdef050_06_rd_uni_rdsft005//sprmdef050_06_rd_uni_rdsft005_g{:03d}.bin",
                # 23. 指し手ルーレット, 個体数50, エリート6, 非独立ランダム選択, 一様交叉, ランダム突変5%のあとに制限無シフト突変5%
                "prm//sprm050_06_rd_uni_rdsft005//sprm050_06_rd_uni_rdsft005_g{:03d}.bin",
                # 24. 指し手ルーレット, 個体数50, エリート6, ランダム突然変異100% (今更だけどエリートは突然変異しないよ)
                "prm//sprm050_06___rd100//sprm050_06___rd100_g{:03d}.bin",
                # 25. 指し手ルーレット, 個体数100, エリート10, 非独立ルーレット選択, 平均と一様一回ずつ, 一様のみランダム突然変異5%
                "prm//sprm100_10_rlt_au_rd005//sprm100_10_rlt_au_rd005_g{:03d}.bin",
                # 26. 圧縮版, 指し手ルーレット, 個体数50, エリート6, 非独立ルーレット選択, 一様交叉, ランダム突然変異5%
                "prm//srltc050_06_rlt_uni_rd005//srltc050_06_rlt_uni_rd005_g{:03d}.bin",
                # 27. 圧縮版, 指し手ルーレット, 個体数50, エリート6, 非独立ルーレット選択, 一点交叉, ランダム突然変異5%
                "prm//srltc050_06_rlt_1p_rd005//srltc050_06_rlt_1p_rd005_g{:03d}.bin",
                # 28. 圧縮版, 指し手ルーレット, 個体数50, エリート6, 非独立ルーレット選択, 平均と一様一回ずつ, それぞれランダム突然変異5%
                "prm//srltc050_06_rlt_aue_rd005//srltc050_06_rlt_aue_rd005_g{:03d}.bin"]

# 圧縮版ファイルのフォーマットの添字はこのリストに加えていく
COMPRESSED_INDICES = [26, 27, 28]

# フォーマットにシードが付いているか
PLUS_SEED = [27, 28]

def main():
    global VIEW_ONLY
    #VIEW_ONLY = False
    ind = 28
    loc_pop = 6
    start_g = 0
    stop_g = 200
    # シードをつけるか否か
    if ind in PLUS_SEED:
        active_format = formatPlusSeed(FILE_FORMATS[ind], 365)
    else:
        active_format = FILE_FORMATS[ind]
    print(active_format)
    # 圧縮添字リストを見てどちらを使うか判断
    if ind in COMPRESSED_INDICES:
        viewMeansGraph(active_format, loc_pop, start_g, stop_g, 1)
        viewSDGraph(active_format, loc_pop, start_g, stop_g, 1)
    else:
        viewMeansGraph(active_format, loc_pop, start_g, stop_g, 0)
        viewSDGraph(active_format, loc_pop, start_g, stop_g, 0)
    #viewSDGraph(FILE_FORMATS[ind], loc_pop, 0, 100)
    #makeJpegFileName(FILE_FORMATS[ind], "means100", 0, 100)
    #imgTest(FILE_FORMATS[ind], 100)
    #makeWinCountFile(FILE_FORMATS[ind], 50, 0, 1000, 0, 100)
    viewWinRateGraph(FILE_FORMATS[ind], 0)
    plt.show()
    print("終わり")

if __name__ == "__main__":
    main()