# グラフ作成ファイル
# ubuntu で使うために分岐した
import json
import re
import os
from ctypes import *
import numpy as np
import math
import statistics as stat
from matplotlib import pyplot as plt

# シード値
SEED = 123

# グラフ描画のみ (画像を保存したい場合は False にする)
VIEW_ONLY = True

# シンプルパラメータの長さ
SPRM_LEN = 10

FILENAME_MAX = 4096

# 共有ライブラリ読み込み(カレントディレクトリを想定)
# ubuntu用であることに注意
share02_ubu = cdll.LoadLibrary(".//share02_ubu.so")

# 配列型定義
FloatArray10 = c_float * 10
FloatArray64 = c_float * 64
IntArray3 = c_int32 * 3
# なんとなく c の FILENAME_MAX に合わせてみた
CharArray4096 = c_char * FILENAME_MAX

# 適応度代入用
# 100 は最大の個体数と考える, 必要があれば調整
# 可変にできないものか…
IntArray100 = c_int32 * 100

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

# 適応度取得関数
getFitness = share02_ubu.getFitnessPy
getFitness.restype = c_int32
getFitness.argtypes = (c_char_p, IntArray100, c_int32)

# ファイル名決定関数
makeSprmFileNameRankGeoProg = share02_ubu.makeSprmFileNameRankGeoProgPy
makeSprmFileNameRankGeoProg.restype = c_int32
makeSprmFileNameRankGeoProg.argtypes = (CharArray4096, c_int32, c_int32, c_int32, c_int32, c_int32, c_double, c_int32)

# 各種ラッパー関数
# n は個体数を指定 (過小はいいがオーバーに注意)
# 圧縮フラグを追加
def getFamilyMeansWrap(fnamer, n, compressed):
    f_arr_c = FloatArray10()
    flag = getFamilyMeans(fnamer.encode(), f_arr_c, n, compressed)
    res_list = []
    # 読み込み成功
    if flag >= 0:
        res_list = list(f_arr_c)
    return res_list

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

# 適応度取得
def getFitnessWrap(fnamer, n):
    # int配列, 余裕を持って多めに確保
    i_arr_c = IntArray100()
    res_list = []
    # エラーじゃなければ必要なサイズにスライスして戻り値とする
    if getFitness(fnamer.encode(), i_arr_c, n) >= 0:
        res_list = list(i_arr_c)[:n]
    return res_list

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

# ファイル名作成
# 個体数, エリート数, 選択ID, 公比, 世代数を与える
# 選択ID: 2: 公比そのまま, 3: 公比自然対数
def makeSprmFileNameRankGeoProgWrap(loc_pop, loc_eln, sel_id, loc_seed, cmn_ratio, gene_num):
    c_arr_c = CharArray4096()
    res_str = ""
    str_len = makeSprmFileNameRankGeoProg(c_arr_c, FILENAME_MAX, loc_pop, loc_eln, sel_id, loc_seed, cmn_ratio, gene_num)
    # 作成に成功したら, バイト型にして複合
    if str_len > 0:
        res_str = bytes(c_arr_c).decode()
    return res_str

# グラフ用の色
LINE_COLORS = [
    "#00ff00", "#1f77b4", "#ff7f0e", "#9467bd", "#ff0000",
    "#8c564b", "#e377c2", "#7f7f7f", "#bcbd22", "#17becf"
]

# グラフを保存するファイル名を決定する関数
# グラフの種類 (平均値や標準偏差), 描画する世代の範囲 (int型) を与える
# シードを含む方をグラフファイル名に使わないと同一扱いされてしまう
# シードが無い場合のグラフを新たに作ると, 旧ファイル名と異なるファイル名になる (_no_seedが付与される)
def makeJpegFileName(fname_format, name, x_min, x_max, x_type="generation"):
    # シード関係なく共通する部分
    # 最初のふるい落とし
    single = False
    m1 = re.match(r'prm(//.+)//(.+)\.bin', fname_format)
    if not m1:
        single = True
        # スラッシュ単体も試す
        m1 = re.match(r'prm(/.+)/(.+)\.bin', fname_format)
        if not m1:
            print("一致するパターンがありません")
            return ""
    mg1 = m1.groups()
    # シードの有無
    m2 = re.match(r'.+(_s[0-9]+)', mg1[1])
    sp = "_no_seed"
    if m2:
        print("シードを含みます")
        sp = m2.groups()[0]
    else:
        print("シードを含みません")
    # スラッシュ単体の場合の処理
    if single:
        path = "/home/sonoda/Pictures/Graphs"
    else:
        path = "//home//sonoda//Pictures//Graphs"
    # x 軸の種類
    # 世代が横軸
    if x_type == "generation":
        path += mg1[0] + "_" + name + "_g{0:03d}-{1:03d}".format(int(x_min), int(x_max)) + sp + ".jpg"
    # 公比が横軸
    elif x_type == "common_ratio":
        #print(mg1[0])
        # 固定された公比の部分は切り抜く
        m3 = re.match(r'(.*rkgexp)([+-][0-9]\.[0-9]{3})(.*)', mg1[0])
        if m3:
            mg3 = m3.groups()
            #print(mg3)
            path += mg3[0] + "{0:+5.3f}{1:+5.3f}".format(float(x_min), float(x_max)) + mg3[2] + name + sp + ".jpg"
        else:
            path = ""
    else:
        path = ""
    print(path)
    return path

# フォーマットにシードも追加
# genetic02 のマクロ名と同じ
def formatPlusSeed(fname_format, seed):
    # .bin とそれ以外を分割
    m = re.match(r'(.+)(\.bin)', fname_format)
    new = ""
    if m:
        mg = m.groups()
        new = mg[0] + "_s{0:03d}".format(seed) + mg[1]
    else:
        print("一致するパターンがありません")
    return new

# 適応度ファイルフォーマットを作成
# シードを付加した後に実行することを想定
# .bin を _format.bin に置き換えるだけ
def makeFitnessFileFormat(fname_format):
    # .bin とそれ以外を分割
    m = re.match(r'(.+)(\.bin)', fname_format)
    if not m:
        print("一致するパターンがありません")
        return ""
    mg = m.groups()
    return mg[0] + "_fitness" + mg[1]

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
    print("平均値の範囲", y_max, y_min)
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
    c = 0
    for i in range(x_min, x_max + 1):
        # i 世代全個体の平均値を取り出す
        tprm = getFamilyMeansWrap(fname_format.format(i), population, compressed)
        # 読み込みエラー
        if not tprm:
            c += 1
            if c >= 10:
                print("有効なパラメータを指定してください")
                return
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
    path = makeJpegFileName(fname_format, "means{:03d}".format(population), x[0], x[-1])
    if path and not VIEW_ONLY:
    # 出力画像の周囲の境界ボックス?を消す
        fig.savefig(path, bbox_inches="tight")
        print("saved!!")

# 標準偏差のグラフを作成
# 強調マスを指定できるようにする
def makeSDGraph(ax, x, ys, emphasize):
    # まずは注目マス以外
    for i in range(SPRM_LEN):
        if i in emphasize:
            continue
        ax.plot(x, ys[i],
            label="{:d}".format(i + 1),
            color=LINE_COLORS[i],
            linewidth=1
        )
    # 注目マス (違うのは先の太さだけ)
    for i in emphasize:
        ax.plot(x, ys[i],
            label="{:d}".format(i + 1),
            color=LINE_COLORS[i],
            linewidth=4
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
# 強調リストを追加
def viewSDGraph(fname_format, population, x_min, x_max, compressed, emphasize=[]):
    x = []
    # 10 マス分のデータの配列を用意
    ys = [[] for i in range(SPRM_LEN)]
    c = 0
    for i in range(x_min, x_max + 1):
        # i 世代全個体の標準偏差を取り出す
        tprm = getFamilySDWrap(fname_format.format(i), population, compressed)
        # 空リストがの場合（読み込み失敗）
        if not tprm:
            c += 1
            if c >= 10:
                print("有効なパラメータを指定してください")
                return
            continue
        # x は読み込めた整数全て
        x.append(i)
        # それぞれのマスの評価値に代入!
        for j in range(10):
            ys[j].append(tprm[j])
    # 読み込めたデータがひとつもない
    if not x:
        return
    fig = plt.figure(figsize=(8, 5))
    ax = fig.add_subplot(111)
    name = "SD{:03d}"
    # 強調マスを番号から添字に変換し, さらに逆順に並べる
    # 1から10以外の無効なマスが与えられたら無視する
    # 重複も無視
    e_valid = []
    if emphasize:
        for i in emphasize:
            # 添字に変換 (-1)
            ind = i - 1
            # 範囲外か重複
            if (ind < 0 or SPRM_LEN <= ind or ind in e_valid):
                continue
            e_valid.insert(0, ind)
        # 有効なマスが存在
        if e_valid:
            name += "_e{:d}".format(e_valid[-1] + 1)
            # 後ろから文字に追加?
            for ind in range(len(e_valid) - 2, -1, -1):
                name += "_{:d}".format(e_valid[ind] + 1)
    makeSDGraph(ax, x, ys, e_valid)
    # フォーマットやグラフの範囲に合わせたパスを作成
    path = makeJpegFileName(fname_format, name.format(population), x[0], x[-1])
    # 書き込み
    if path and not VIEW_ONLY:
        fig.savefig(path, bbox_inches="tight")
        print("saved!!")

# 2つのグラフを同時描画したい
# 圧縮フラグ追加
# 強調リストを追加 (初期値は空)
def viewStatGraphs(fname_format, population, g_min, g_max, compressed, emphasize=[]):
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
    makeSDGraph(ax2, g, SD, emphasize)
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

# 適応度のグラフを作成
# データとグラフオブジェクト? を渡す
# 横軸も指定可能, 縦軸の凡例も指定可能 (データと数が合うように!)
def makeFitnessGraph(ax, x, ys, x_label="generation", y_labels=("max", "median", "min")):
    # 縦軸の値の間隔
    step = 20
    # 各データのプロットとラベル指定
    for i, j in enumerate(y_labels):
        ax.plot(x, ys[i], label=j, marker="o", markeredgewidth=0)
    # 凡例調節
    ax.legend(bbox_to_anchor=(1.01, 1), loc='upper left', borderaxespad=0, fontsize=10)
    #ax.grid()
    # ラベル指定
    ax.set_xlabel(x_label, fontsize=15)
    ax.set_ylabel("point", fontsize=15)
    # 横幅指定（読み込みに成功したデータだけ）
    ax.set_xticks(np.linspace(x[0], x[-1], 11))
    # 最大値の最大値から上限を決める
    y_max = math.ceil(max(ys[0]) / step) * step
    # 最小値の最小値から下限を決める
    y_min = math.floor(min(ys[2]) / step) * step
    print("得点の範囲", y_max, y_min)
    # 縦幅指定 (間隔は固定)
    ax.set_yticks(np.arange(y_min, y_max + step / 2, step))

# 適応度を見るグラフ
# これは世代全体を見ること前提
# 最大値, 中央値, 最小値でも表示してみようかな?
def viewFitnessGraph(fff, loc_pop, g_min, g_max):
    # 世代数 (読み込めた数値だけ格納)
    x = []
    # 3 つの配列を用意
    ys = [[], [], []]
    c = 0
    for i in range(g_min, g_max + 1):
        # i 世代の適応度を取り出す
        fl = getFitnessWrap(fff.format(i), loc_pop)
        if not fl:
            c += 1
            if c >= 10:
                print("有効なパラメータを指定してください")
                return
            continue
        # 読み込めたらその世代を x に追加
        x.append(i)
        # 最大値を格納
        ys[0].append(max(fl))
        # 中央値を格納 (ソート済だから非効率かもね)
        ys[1].append(stat.median(fl))
        # 最小値を格納
        ys[2].append(min(fl))
    if not x:
        return
    fig = plt.figure(figsize=(8, 5))
    ax = fig.add_subplot(111)
    # グラフファイル名作成時にfitnessが除外されるのでここで追加
    name = "fitness{:03d}".format(loc_pop)
    makeFitnessGraph(ax, x, ys)
    # フォーマット, データの種類, グラフの範囲に合わせたパスを作成
    path = makeJpegFileName(fff, name, int(x[0]), int(x[-1]))
    # 書き込み
    if path and not VIEW_ONLY:
        fig.savefig(path, bbox_inches="tight")
        print("saved!!")

# 横軸公比の自然対数, 縦軸適応度のグラフを作成
# 世代数は固定
def viewFitnessGraph2(loc_pop, loc_eln, lncr_start, lncr_stop, lncr_step, gene_num, loc_seed):
    x = []
    ys = [[], [], []]
    n = int((lncr_stop - lncr_start) / lncr_step)
    c = 0
    for i in range(n + 1):
        lncr = lncr_start + lncr_step * i
        # ファイル名作成 (適応度ファイルではない)
        fname = makeSprmFileNameRankGeoProgWrap(loc_pop, loc_eln, 3, loc_seed, lncr, gene_num)
        # 適応度取得
        fl = getFitnessWrap(makeFitnessFileFormat(fname), loc_pop)
        if fl:
            c = 0
            x.append(lncr)
            ys[0].append(max(fl))
            ys[1].append(stat.median(fl))
            ys[2].append(min(fl))
        else:
            c += 1
            if c >= 10:
                print("有効なパラメータを指定してください")
                return
    if not x:
        return
    fig = plt.figure(figsize=(8, 5))
    ax = fig.add_subplot(111)
    makeFitnessGraph(ax, x, ys, x_label="the natural log of common ratio")
    #fname = makeSprmFileNameRankGeoProgWrap(loc_pop, loc_eln, 3, loc_seed, -0.005, gene_num)
    # いくつかオプションはここで決めておく
    name = "_g{:03d}_smp{:03d}".format(gene_num, loc_pop)
    path = makeJpegFileName(fname, name, x[0], x[-1], x_type="common_ratio")
    if not VIEW_ONLY:
        fig.savefig(path, bbox_inches="tight")
        print("saved!")

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
                # gの付け忘れ!
                "prm//srltc050_06_rlt_uni_rd005//srltc050_06_rlt_uni_rd005_{:03d}.bin",
                # 27. 圧縮版, 指し手ルーレット, 個体数50, エリート6, 非独立ルーレット選択, 一点交叉, ランダム突然変異5%
                "prm//srltc050_06_rlt_1p_rd005//srltc050_06_rlt_1p_rd005_g{:03d}.bin",
                # 28. 圧縮版, 指し手ルーレット, 個体数50, エリート6, 非独立ルーレット選択, 平均と一様一回ずつ, それぞれランダム突然変異5%
                "prm//srltc050_06_rlt_aue_rd005//srltc050_06_rlt_aue_rd005_g{:03d}.bin",
                # 29. 圧縮版, 対ランダム, 指し手固定, 個体数50, エリートなし, 等比数列ランキング選択公比0.90, 2人っ子一様交叉, ラン突1%
                "prm//srddc050__rkg90_uni2_rd001//srddc050__rkg90_uni2_rd001_g{:03d}.bin",
                # 30. 圧縮版, 対ランダム, 指し手固定, 個体数50, エリートなし, 等比数列ランキング選択公比0.80, 2人っ子一様交叉, ラン突1%
                "prm//srddc050__rkg80_uni2_rd001//srddc050__rkg80_uni2_rd001_g{:03d}.bin",
                # 31. 圧縮版, 対ランダム, 指し手固定, 個体数50, エリートなし, 等比数列ランキング選択公比0.50, 2人っ子一様交叉, ラン突1%
                "prm//srddc050__rkg50_uni2_rd001//srddc050__rkg50_uni2_rd001_g{:03d}.bin",
                # 32. 公比0.20
                "prm//srddc050__rkg20_uni2_rd001//srddc050__rkg20_uni2_rd001_g{:03d}.bin",
                # 33. 公比0.99
                "prm//srddc050__rkg99_uni2_rd001//srddc050__rkg99_uni2_rd001_g{:03d}.bin",
                # 34. 公比 exp(-0.005)
                "prm//srddc050__rkgexp-0.005_uni2_rd001//srddc050__rkgexp-0.005_uni2_rd001_g{:03d}.bin",
                # 35. 公比 exp(-0.020)
                "prm//srddc050__rkgexp-0.020_uni2_rd001//srddc050__rkgexp-0.020_uni2_rd001_g{:03d}.bin"]

# 圧縮版ファイルのフォーマットの添字はこのリストに加えていく
# そろそろめんどい
COMPRESSED_INDICES = [26, 27, 28, 29, 30, 31, 32, 33, 34, 35]

# 各フォーマットでどのシードを使ったのか記録
# フォーマットにシードが付いているかの判定に使える
SEED_DICT = {27: (123, 365, 999), 28: (365,), 29: (123, 999), 30: (123,), 31: (123,), 32: (123,), 33: (123,), 34: (123,), 35: (123,)}

def old():
    ind = 17
    loc_pop = 50
    start_g = 0
    stop_g = 100
    chumoku = []
    # シードをつけるか否か
    if ind in SEED_DICT:
        # シードがある場合はここで指定
        active_format = formatPlusSeed(FILE_FORMATS[ind], SEED_DICT[ind][0])
    else:
        active_format = FILE_FORMATS[ind]
    print("フォーマット: ", active_format)
    # 圧縮添字リストを見てどちらを使うか判断
    if ind in COMPRESSED_INDICES:
        viewMeansGraph(active_format, loc_pop, start_g, stop_g, 1)
        viewSDGraph(active_format, loc_pop, start_g, stop_g, 1, chumoku)
        # 圧縮版なら適応度ファイルもあるやろ多分
        viewFitnessGraph(makeFitnessFileFormat(active_format), loc_pop, start_g, stop_g)
    else:
        viewMeansGraph(active_format, loc_pop, start_g, stop_g, 0)
        viewSDGraph(active_format, loc_pop, start_g, stop_g, 0, chumoku)

def main():
    global VIEW_ONLY
    # 画像保存する場合はこのコメントアウトを外す
    #VIEW_ONLY = False
    loc_pop = 50
    loc_eln = 1
    #old()
    viewFitnessGraph2(loc_pop, loc_eln, -0.1, 0.1, 0.005, 10, 123)
    plt.show()
    print("終わり")

if __name__ == "__main__":
    main()