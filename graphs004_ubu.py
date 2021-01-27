# グラフ作成ファイル
# ubuntu で使うために分岐した
import json
import re
import os
import numpy as np
import math
import statistics as stat
from matplotlib import pyplot as plt
# ctypes とのインターフェースのつもり
import share_lib01 as sl

# ラッパー関数の集まりのインスタンス
slw = sl.ShareLibWrap()

# シード値 (ランダム対戦ファイル作成用)
SEED = 123

# 適応度の最大値
FITNESS_MAX = 200

# グラフ描画のみ (画像を保存したい場合は False にする)
VIEW_ONLY = True

# グラフ用の色
LINE_COLORS = [
    "#00ff00", "#1f77b4", "#ff7f0e", "#9467bd", "#ff0000",
    "#8c564b", "#e377c2", "#7f7f7f", "#bcbd22", "#17becf"
]

# LaTeX で使う図を保存するパス
FIGURES_PATH = "/home/sonoda/GitHub/HelloGitHub/LaTeX/figures"
# テスト用
FIGURES_TEST_PATH = "./fig_test"

# グラフを保存するファイル名を決定する関数
# グラフの種類 (平均値や標準偏差), 描画する世代の範囲 (int型) を与える
# ディレクトリのパスも追加, 拡張子を与えることで, 様々なファイル形式に対応可能?
def makeGraphsFileName(fname_format, name, x_min, x_max, x_type="generation", extention="jpg", c_map=False, dir_path=FIGURES_TEST_PATH):
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
        #print("シードを含みます")
        sp = m2.groups()[0]
    else:
        print("シードを含みません")
    # スラッシュ単体の場合の処理
    if single:
        #path = "/home/sonoda/Pictures/Graphs"
        path = dir_path
    else:
        path = "//home//sonoda//Pictures//Graphs"
    # 世代が横軸. ただしカラーマップの場合は例外となる (ごめんなさい).
    if x_type == "generation":
        # カラーマップの場合, 固定された公比の部分は排除 (ランキングの部分は残す).
        if c_map:
            m3 = re.match(r'(.*rkg)(exp[+-][0-9]\.[0-9]{3})(.*)', mg1[0])
            if m3:
                mg3 = m3.groups()
                #print(mg3)
                path += mg3[0] + mg3[2] + "_" + name + "_g{0:03d}-{1:03d}".format(int(x_min), int(x_max)) + sp
            else:
                path = ""
        else:
            path += mg1[0] + "_" + name + "_g{0:03d}-{1:03d}".format(int(x_min), int(x_max)) + sp
    # 公比が横軸
    elif x_type == "common_ratio":
        #print(mg1[0])
        # 固定された公比の部分は切り抜く
        m3 = re.match(r'(.*rkgexp)([+-][0-9]\.[0-9]{3})(.*)', mg1[0])
        if m3:
            mg3 = m3.groups()
            print(mg3)
            path += mg3[0] + "{0:+5.3f}{1:+5.3f}".format(float(x_min), float(x_max)) + mg3[2] + name + sp
        else:
            path = ""
    else:
        path = ""
    # 拡張子を付加
    path += "." + extention
    print(path)
    return path

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

# でたらめなファイル名を作る
# 最後に付加できるように引数を追加しておく
def makeJsonFileName2(loc_pop, loc_eln, loc_seed, start_cr_th, stop_cr_th, step_cr_th, gene_num, vs_seed, loop, dnfunc_id, name=""):
    json_fname = "json/wp{:03d}_{:02d}_seed{:03d}_{:+05d}_{:+05d}_{:04d}_{:03d}_".format(loc_pop, loc_eln, loc_seed, start_cr_th, stop_cr_th, step_cr_th, gene_num) + name + "_"
    if (dnfunc_id == 0):
        json_fname += "d"
    else:
        json_fname += "r_vseed{:03d}_loop{:04d}".format(vs_seed, loop)
    json_fname += ".json"
    print(json_fname)
    return json_fname

# 公比同士対戦結果のグラフを保存するファイル名
# デフォの拡張子は pdf
def makePVSOCRGraphFileName(json_fname, extention="pdf"):
    path = FIGURES_PATH
    m1 = re.match(r'(json)(/.*)(\.json)', json_fname)
    if not m1:
        return ""
    mg1 = m1.groups()
    #print(mg1)
    path += mg1[1] + "." + extention
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

# 既に固定されている世代を, 書き換えられるように変更
def makeGeneVariable(fname):
    m = re.match(r'(.+g)([0-9]{3})(.+)', fname)
    gene_flex = ""
    if m:
        mg = m.groups()
        #print(mg)
        gene_flex = mg[0] + "{:03d}" + mg[2]
    else:
        print("一致するパターンがありません")
    return gene_flex

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
        ax.plot(x, ys[i], label="{:d}".format(i + 1), color=lc, linewidth=lw)
    #plt.legend(loc="best")
    # 凡例調節
    ax.legend(bbox_to_anchor=(1.01, 1), loc='upper left', borderaxespad=0, fontsize=10)
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
        tprm = slw.getFamilyMeansWrap(fname_format.format(i), population, compressed)
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
    path = makeGraphsFileName(fname_format, "means{:03d}".format(population), x[0], x[-1])
    if path and not VIEW_ONLY:
    # 出力画像の周囲の境界ボックス?を消す
        fig.savefig(path, bbox_inches="tight")
        print("saved!!")

# 標準偏差のグラフを作成
# 強調マスを指定できるようにする
def makeSDGraph(ax, x, ys, emphasize):
    # まずは注目マス以外
    for i in range(sl.SPRM_LEN):
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
    ys = [[] for i in range(sl.SPRM_LEN)]
    c = 0
    for i in range(x_min, x_max + 1):
        # i 世代全個体の標準偏差を取り出す
        tprm = slw.getFamilySDWrap(fname_format.format(i), population, compressed)
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
            if (ind < 0 or sl.SPRM_LEN <= ind or ind in e_valid):
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
    path = makeGraphsFileName(fname_format, name.format(population), x[0], x[-1])
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
        tmp1 = slw.getFamilyMeansWrap(fname, population, compressed)
        # 空リストがの場合（読み込み失敗）
        # 平均値が読み込める=標準偏差も読み込めると考える
        if not tmp1:
            continue
        # 標準偏差も読み込む
        tmp2 = slw.getFamilySDWrap(fname, population, compressed)
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

# 他の公比と対戦したときの勝ち点
def viewPointVSOtherCR(loc_pop, loc_eln, loc_seed, start_cr_th, stop_cr_th, step_cr_th, gene_num, vs_seed, loop, dnfunc_id, zako=0, name=""):
    if zako:
        name2 = name + "_zako"
    else:
        name2 = name
    json_fname = makeJsonFileName2(loc_pop, loc_eln, loc_seed, start_cr_th, stop_cr_th, step_cr_th, gene_num, vs_seed, loop, dnfunc_id, name=name2)
    del name2
    first = True
    # ファイルが存在
    if os.path.exists(json_fname):
        print("既存")
        f = open(json_fname, "r")
        pt_list = json.load(f)
        f.close()
        first = False
    else:
        print("初めて")
        pt_list = slw.vsOtherCommonRatioWrap(loc_pop, loc_eln, loc_seed, start_cr_th, stop_cr_th, step_cr_th, gene_num, vs_seed, loop, dnfunc_id, zako)
    print(pt_list)
    # 初めてならファイル書き込み
    if first:
        f = open(json_fname, "w")
        json.dump(pt_list, f)
        f.close()
    x = [i / 1000 for i in range(start_cr_th, stop_cr_th, step_cr_th)]
    x.append(stop_cr_th / 1000)
    fig, ax = makeSimpleGraph(x, pt_list, x_label="the natural log of common ratio", y_label="point", option="plot")
    path = makePVSOCRGraphFileName(json_fname)
    if name == "grid":
        ax.grid()
    saveFigWrap(fig, path)

# 単純な2次元グラフを作成
def makeSimpleGraph(x, y, mag=0.65, x_label="median fitness", y_label="mean weight", option="scatter"):
    plt.rcParams["font.size"] = 12 * mag
    fig = plt.figure(figsize=(8 * mag, 5 * mag))
    ax = fig.add_subplot(111)
    ax.set_xlabel("the natural log of common ratio", fontsize=15*mag)
    ax.set_ylabel("point", fontsize=15*mag)
    plt.xticks(fontsize=12*mag)
    #ax.set_xticks([j for i, j in enumerate(x) if i % 5 == 0])
    plt.yticks(fontsize=12*mag)
    # 散布図
    if option == "scatter":
        ax.scatter(x, y, s=3)
    else:
        ax.plot(x, y)
    fig.tight_layout()
    return (fig, ax)

# 適応度の中央値と, 重みの平均値とのグラフを作りたい
# 相関を確かめるため
def viewFitMedWMeanGraph(loc_pop, loc_eln, crs_id, lncr_start, lncr_stop, lncr_step, g_min, g_max, loc_seed, w_num=1, options=0b00, mag=0.65):
    w_idx = int(w_num) - 1 # 添字に変換
    if w_idx < 0 or 9 < w_idx:
        print("有効な重み番号を指定してください")
        return
    fname = slw.makeSprmFileNameRankGeoProgWrap(loc_pop, loc_eln, 3, crs_id, loc_seed, 0.0, 0, options=options)
    name1 = "fit_med_map_rexp{:+5.3f}{:+5.3f}_res{:4.3f}".format(lncr_start, lncr_stop, lncr_step)
    json_fname1 = makeGraphsFileName(fname, name1, g_min, g_max, c_map=True, extention="json", dir_path="./json")
    fmedl = makeOrLoadCmapData(json_fname1, loc_pop, loc_eln, crs_id, lncr_start, lncr_stop, lncr_step, g_min, g_max, loc_seed, options=options)[2]
    # 重み, 公比情報
    name2 = "w{:02d}mean_map_rexp{:+5.3f}{:+5.3f}_res{:5.3f}".format(w_idx + 1, lncr_start, lncr_stop, lncr_step)
    json_fname2 = makeGraphsFileName(fname, name2, g_min, g_max, c_map=True, extention="json", dir_path="./json")
    wml = makeOrLoadCmapData(json_fname2, loc_pop, loc_eln, crs_id, lncr_start, lncr_stop, lncr_step, g_min, g_max, loc_seed, options=options, data_option="w_mean", w_idx=w_idx)[2]
    makeSimpleGraph(fmedl, wml)
    coef = np.corrcoef(fmedl, wml)
    print(coef)

# 適応度のグラフを作成
# データとグラフオブジェクト? を渡す
# 横軸も指定可能, 縦軸の凡例も指定可能 (データと数が合うように!)
def makeFitnessGraph(ax, x, ys, x_label="generation", y_labels=("max", "median", "min"), grid=False):
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
    # グリッド線
    if grid:
        ax.grid()

# 適応度を見るグラフ
# これは世代全体を見ること前提
# 最大値, 中央値, 最小値でも表示してみようかな?
def viewFitnessGraph(fff, loc_pop, g_min, g_max, grid=False):
    # 世代数 (読み込めた数値だけ格納)
    x = []
    # 3 つの配列を用意
    ys = [[], [], []]
    c = 0
    for i in range(g_min, g_max + 1):
        # i 世代の適応度を取り出す
        fl = slw.getFitnessWrap(fff.format(i), loc_pop)
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
    makeFitnessGraph(ax, x, ys, grid=grid)
    # フォーマット, データの種類, グラフの範囲に合わせたパスを作成
    path = makeGraphsFileName(fff, name, x[0], x[-1])
    # 書き込み
    if path and not VIEW_ONLY:
        if os.path.exists(path):
            if input(path + " は存在します. 上書きしますか? (y\\n): ") != "y":
                return
        fig.savefig(path, bbox_inches="tight")
        print("saved!!")

# 横軸公比の自然対数, 縦軸適応度のグラフを作成
# 世代数は固定, グリッド線をオプションで追加
# stop も含める! 多分
# 交叉方法も指定
def viewFitnessGraph2(loc_pop, loc_eln, crs_id, lncr_start, lncr_stop, lncr_step, gene_num, loc_seed, fg=0, grid=False):
    x = []
    ys = [[], [], []]
    # stop まで含めた数
    n = int((lncr_stop - lncr_start) / lncr_step) + 1
    c = 0
    for i in range(n):
        lncr = lncr_start + lncr_step * i
        # ファイル名作成 (適応度ファイルではない)
        fname = slw.makeSprmFileNameRankGeoProgWrap(loc_pop, loc_eln, 3, crs_id, loc_seed, lncr, gene_num)
        # 適応度取得
        fl = slw.getFitnessWrap(makeFitnessFileFormat(fname), loc_pop)
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
    makeFitnessGraph(ax, x, ys, x_label="the natural log of common ratio", grid=grid)
    #fname = makeSprmFileNameRankGeoProgWrap(loc_pop, loc_eln, 3, loc_seed, -0.005, gene_num)
    # いくつかオプションはここで決めておく
    name = "_g{:03d}_smp{:03d}".format(gene_num, loc_pop)
    if grid:
        name += "_grid"
    # 最後のファイル名をパス作成に使う (公比は範囲に置き換えられる)
    #print(fname)
    path = makeGraphsFileName(fname, name, x[0], x[-1], x_type="common_ratio")
    if not VIEW_ONLY:
        if os.path.exists(path):
            if input(path + " は存在します. 上書きしますか? (y\\n): ") != "y":
                return
        fig.savefig(path, bbox_inches="tight")
        print("saved!")

# フォーマットを自動作成し, あとは本家適応度-世代数グラフ作成関数におまかせ
# オプションでは正規化フラグ, 初期世代設定フラグを与える
def viewFitnessGraph3(loc_pop, smp_num, loc_eln, crs_id, lncr, g_min, g_max, loc_seed, options=0b10, grid=False):
    # 0 世代でファイル名作成, 世代を可変にし, fitness を付加
    fff = makeFitnessFileFormat(makeGeneVariable(slw.makeSprmFileNameRankGeoProgWrap(loc_pop, loc_eln, 3, crs_id, loc_seed, lncr, 0, options)))
    print(fff)
    viewFitnessGraph(fff, smp_num, g_min, g_max, grid=grid)

# カラーマップデータを作るか, ロードする
def makeOrLoadCmapData(json_fname, loc_pop, loc_eln, crs_id, lncr_start, lncr_stop, lncr_step, g_min, g_max, loc_seed, options=0b00, data_option="fit_median", w_idx=0, forced=False):
    x = np.arange(lncr_start, lncr_stop + lncr_step, lncr_step)
    y = np.arange(g_min, g_max + 1, 1, np.int32)
    # ファイルが既存ならロードして戻るだけ
    if not forced and os.path.exists(json_fname):
        print("既存")
        f = open(json_fname, "r")
        z = json.load(f)
        f.close()
    else:
        # 適応度が欲しいとき
        m1 = re.match(r'(fit_)(.*)', data_option)
        if m1:
            stat_option = m1.groups()[1]
            if stat_option == "median":
                selDatFunc = lambda l: stat.median(l)
            elif stat_option == "SD":
                selDatFunc = lambda l: stat.stdev(l)
            else:
                print("無効な統計値")
                return
            getDatFunc = lambda fname, loc_pop: slw.getFitnessWrap(makeFitnessFileFormat(fname), loc_pop)
        # いずれかの重みが欲しい時
        else:
            if data_option == "w_SD": # 各重みごとの標準偏差の平均値
                getDatFunc = lambda fname, loc_pop: slw.getFamilySDWrap(fname, loc_pop, 1)
                selDatFunc = lambda l: stat.mean(l)
            elif data_option == "w_mean": # 指定した重みの平均値
                getDatFunc = lambda fname, loc_pop: slw.getFamilyMeansWrap(fname, loc_pop, 1)
                selDatFunc = lambda l: l[w_idx]
        if forced:
            print("強制")
        else:
            print("初めて")
        z = []
        c = 0
        for lncr in x:
            for gene_num in y:
                fname = slw.makeSprmFileNameRankGeoProgWrap(loc_pop, loc_eln, 3, crs_id, loc_seed, lncr, gene_num, options=options)
                tmpl = getDatFunc(fname, loc_pop)
                if not tmpl:
                    zi = -1
                    c += 1
                    if c >= 10:
                        print("有効なパラメータを指定してください")
                        return
                else:
                    c = 0
                    zi = selDatFunc(tmpl)
                z.append(zi)
        f = open(json_fname, "w")
        json.dump(z, f)
        f.close()
    return (x, y, z)

# 公比と世代数を軸とし, 適応度に応じて色が異なるカラーマップを作成
def viewFitnessGraph4(loc_pop, loc_eln, crs_id, lncr_start, lncr_stop, lncr_step, g_min, g_max, loc_seed, options=0b00, stat_option="median", mag=0.65):
    name1 = "fit_"
    if stat_option == "median":
        name1 += "med"
        cm = plt.cm.get_cmap("RdYlGn")
        v_min = 0
        v_max = FITNESS_MAX
    elif stat_option == "stdev":
        name1 += "SD"
        cm = plt.cm.get_cmap("inferno")
    elif stat_option == "variance":
        name1 += "variance"
        cm = plt.cm.get_cmap("inferno")
    else:
        print("不明な統計値")
        return
    # json ファイル名作成
    fname = slw.makeSprmFileNameRankGeoProgWrap(loc_pop, loc_eln, 3, crs_id, loc_seed, 0.0, 0, options=options)
    name2 = name1 + "_map_rexp{:+5.3f}{:+5.3f}_res{:4.3f}".format(lncr_start, lncr_stop, lncr_step)
    json_fname = makeGraphsFileName(fname, name2, g_min, g_max, c_map=True, extention="json", dir_path="./json")
    x, y, z = makeOrLoadCmapData(json_fname, loc_pop, loc_eln, crs_id, lncr_start, lncr_stop, lncr_step, g_min, g_max, loc_seed, options=options, data_option=name1)
    # 標準偏差, 分散の最大値は可変
    if stat_option in ("stdev", "variance"):
        v_min = 0
        v_max = max(z)
    fig = makeCmap(x, y, z, v_min, v_max, mag=mag, c_pattern=cm)[0]
    # グラフのサイズの倍率もファイル名に加える
    name3 = name2 + "_size{:4.2f}".format(mag)
    path = makeGraphsFileName(fname, name3, g_min, g_max, extention="pdf", c_map=True)
    saveFigWrap(fig, path)

# 確認とかいろいろしてから図を保存
def saveFigWrap(fig, path):
    if not VIEW_ONLY and path:
        if os.path.exists(path):
            if input(path + " は存在します. 上書きしますか? (y\\n): ") != "y":
                return
        fig.savefig(path, bbox_inches="tight")
        print("保存されました")

# 各重みの標準偏差の平均値を見たい
# 多様性として捉えていいのか?
def viewWeightSDMeansMap(loc_pop, loc_eln, crs_id, lncr_start, lncr_stop, lncr_step, g_min, g_max, loc_seed, options=0b00, mag=0.65):
    # json ファイル名作成
    fname = slw.makeSprmFileNameRankGeoProgWrap(loc_pop, loc_eln, 3, crs_id, loc_seed, 0.0, 0, options=options)
    name1 = "wsdm_map_rexp{:+5.3f}{:+5.3f}_res{:4.3f}".format(lncr_start, lncr_stop, lncr_step)
    json_fname = makeGraphsFileName(fname, name1, g_min, g_max, c_map=True, extention="json", dir_path="./json")
    x, y, z = makeOrLoadCmapData(json_fname, loc_pop, loc_eln, crs_id, lncr_start, lncr_stop, lncr_step, g_min, g_max, loc_seed, options=options, data_option="w_SD")
    fig = makeCmap(x, y, z, 0, max(z), mag=mag, c_pattern="plasma")[0]
    name2 = name1 + "_size{:4.2f}".format(mag)
    path = makeGraphsFileName(fname, name2, g_min, g_max)
    saveFigWrap(fig, path)

# 指定した重みの平均値を見たい
# w_num は重み番号 (1 から 10 を与える)
def viewWeightMeansMap(loc_pop, loc_eln, crs_id, lncr_start, lncr_stop, lncr_step, g_min, g_max, loc_seed, w_num=1, options=0b00, mag=0.65):
    w_idx = int(w_num) - 1 # 添字に変換
    if w_idx < 0 or 9 < w_idx:
        print("有効な重み番号を指定してください")
        return
    # 重み, 公比情報
    name1 = "w{:02d}mean_map_rexp{:+5.3f}{:+5.3f}_res{:5.3f}".format(w_idx + 1, lncr_start, lncr_stop, lncr_step)
    fname = slw.makeSprmFileNameRankGeoProgWrap(loc_pop, loc_eln, 3, crs_id, loc_seed, 0.0, 0, options=options)
    json_fname = makeGraphsFileName(fname, name1, g_min, g_max, c_map=True, extention="json", dir_path="./json")
    # 軸となる配列
    x, y, z = makeOrLoadCmapData(json_fname, loc_pop, loc_eln, crs_id, lncr_start, lncr_stop, lncr_step, g_min, g_max, loc_seed, options=options, data_option="w_mean", w_idx=w_idx)
    fig = makeCmap(x, y, z, -0.5, 0.5, mag=mag, c_pattern="RdYlBu")[0]
    # 倍率情報の付加
    name2 = name1 + "_size{:4.2f}".format(mag)
    path = makeGraphsFileName(fname, name2, g_min, g_max, c_map=True, extention="pdf")
    # 図の保存 (グローバル変数や標準入力で上書き警告などする)
    saveFigWrap(fig, path)

# カラーマップの作成
def makeCmap(x, y, z, z_min, z_max, mag=0.65, c_pattern="RdYlGn"):
    X, Y = np.meshgrid(x, y)
    Z = np.array(z).reshape(len(x), -1).T
    plt.rcParams["font.size"] = 12 * mag
    fig = plt.figure(figsize=(8 * mag, 5 * mag))
    ax = fig.add_subplot(111)
    ax.set_xlabel("the natural log of common ratio", fontsize=15*mag)
    ax.set_ylabel("generation", fontsize=15*mag)
    plt.xticks(fontsize=12*mag)
    ax.set_xticks([j for i, j in enumerate(x) if i % 5 == 0])
    plt.yticks(fontsize=12*mag)
    mappable = ax.pcolor(X, Y, Z, vmin=z_min, vmax=z_max, cmap=c_pattern, shading="nearest")
    fig.colorbar(mappable, ax=ax)
    fig.tight_layout()
    # 図と座標? を返す
    return (fig, ax)

def main():
    global VIEW_ONLY
    # 画像保存する場合はこのコメントアウトを外す
    VIEW_ONLY = False
    #population = 50
    #elite_num = 0
    #lncr = -2
    #gene_num = 10
    #crs_id = 5
    #seed = 123
    #viewFitnessGraph2(population, elite_num, crs_id, -0.02, 0.02, 0.001, gene_num, seed, grid=True)
    #viewFitnessGraph3(population, 50, elite_num, crs_id, lncr, 0, 100, seed, grid=True, fg=1)
    #viewFitnessGraph4(50, 0, 5, -2.0, 0.0, 0.02, 0, 50, seed, 0b01)
    # 標準・幅最小
    #viewFitnessGraph4(50, 0, 5, -0.02, 0.02, 0.001, 0, 100, 555, options=0b00)
    #viewFitnessGraph4(50, 0, 5, -0.02, 0.02, 0.001, 0, 100, 555, options=0b00, stat_option="stdev")
    #viewWeightMeansMap(50, 0, 5, -0.02, 0.02, 0.001, 0, 100, 555, w_num=1, options=0b00)
    #viewWeightSDMeansMap(50, 0, 5, -0.02, 0.02, 0.001, 0, 100, 555, options=0b00)
    #viewFitMedWMeanGraph(50, 0, 5, -0.02, 0.02, 0.001, 0, 100, 555, w_num=2)
    #viewFitnessGraph4(50, 0, 5, -0.1, 0.1, 0.005, 0, 100, 555, options=0b00)
    #viewFitnessGraph4(50, 0, 5, -0.1, 0.1, 0.005, 0, 100, 555, options=0b00, stat_option="stdev")
    #viewFitnessGraph4(50, 0, 5, -2, 2, 0.1, 0, 100, 555, options=0b00)
    #viewFitnessGraph4(50, 0, 5, -2, 2, 0.1, 0, 100, 555, options=0b00, stat_option="stdev")
    #viewFitnessGraph4(50, 1, 5, -0.02, 0.02, 0.001, 0, 100, 123, 0b00)
    #viewFitnessGraph4(50, 1, 5, -0.02, 0.02, 0.001, 0, 100, 123, options=0b00, stat_option="stdev")
    #viewFitnessGraph4(50, 0, 5, -2.0, 0.0, 0.05, 0, 100, 555, options=0b01)
    # 初期雑魚
    #viewFitnessGraph4(50, 0, 5, -2.0, 0.0, 0.05, 0, 100, 555, options=0b01, stat_option="stdev")
    #viewWeightMeansMap(50, 0, 5, -2.0, 0.0, 0.05, 0, 100, 555, w_num=6, options=0b01)
    #viewFitMedWMeanGraph(50, 0, 5, -2.0, 0.0, 0.05, 0, 100, 555, w_num=9, options=0b01)
    #viewWeightSDMeansMap(50, 0, 5, -2.0, 2.0, 0.1, 0, 100, 555, options=0b00)
    #viewWeight1MeansMap(50, 0, 5, -0.1, 0.1, 0.005, 0, 100, 555, options=0b00)
    #viewWeightSDMeansMap(50, 0, 5, -0.1, 0.1, 0.005, 0, 100, 555, options=0b00)
    #viewWeight1MeansMap(50, 0, 5, -2, 2, 0.1, 0, 100, 555, options=0b00)
    #viewWeightSDMeansMap(50, 0, 5, -8, 8, 0.4, 0, 100, 555, options=0b00)
    #viewWeight1MeansMap(50, 0, 5, -8, 8, 0.4, 0, 100, 555, options=0b00)
    #viewFitnessGraph4(50, 0, 5, -8.0, 8.0, 0.4, 0, 100, 555, 0b00)
    #viewFitnessGraph4(50, 0, 5, -0.02, 0.02, 0.001, 0, 100, 555, 0b00, stat_option="median")
    #viewFitnessGraph4(50, 0, 5, -0.1, 0.1, 0.005, 0, 100, 555, 0b00)
    #viewFitnessGraph4(50, 0, 5, -2, 2, 0.1, 0, 100, 555, 0b00, stat_option="variance")
    # ファイル名作成テスト
    #fname = slw.makeSprmFileNameRankGeoProgWrap(50, 1, 3, 5, 123, 2.0, 100, options=0b10)
    #path = makeGraphsFileName(fname, "dummy", 0, 100, extention="pdf", c_map=True)
    #makeJsonFileName2(50, 0, 555, -2000, 2000, 100, 100, 123, 10, 1)
    #viewPointVSOtherCR(50, 0, 555, -100, 100, 5, 100, 365, 10, 1, 0)
    #viewPointVSOtherCR(50, 0, 555, -2000, 0, 50, 100, 123, 100, 1, 1)
    #viewPointVSOtherCR(50, 0, 555, -2000, 0, 50, 100, 123, 0, 0, 1, name="grid")
    viewPointVSOtherCR(50, 0, 555, -8000, 8000, 400, 100, 123, 10, 0, zako=0)
    plt.show()
    print("終わり")

if __name__ == "__main__":
    main()