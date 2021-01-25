import ctypes

# シンプルパラメータの長さ
SPRM_LEN = 10
# ファイル (パス) 名の長さの最大値を適当に決めておく (c のマクロと同じ?)
FILENAME_MAX = 4096

# 共有ライブラリ読み込み(カレントディレクトリを想定)
# ubuntu用であることに注意
share02_ubu = ctypes.cdll.LoadLibrary(".//share02_ubu.so")
# 公比同士対専用
compare01_share_ubu = ctypes.cdll.LoadLibrary(".//compare01_share_ubu.so")

# 配列型定義
FloatArray10 = ctypes.c_float * 10
FloatArray64 = ctypes.c_float * 64
IntArray3 = ctypes.c_int32 * 3
# 盤面情報代入用
IntArray64 = ctypes.c_int32 * 64
# 適応度代入用 (100 は今まで扱った個体数の最大値)
IntArray100 = ctypes.c_int32 * 100

# なんとなく c の FILENAME_MAX に合わせてみた
CharArray4096 = ctypes.c_char * FILENAME_MAX

# 初期化関数を実行 (import 時に実行してくれるかな?)
share02_ubu.initPy()
# こっちの初期化関数も
compare01_share_ubu.initPy2()

# 全個体の平均値
# 第四引数に圧縮フラグを設定
getFamilyMeans = share02_ubu.getFamilyMeansPy
getFamilyMeans.restype = ctypes.c_int32
getFamilyMeans.argtypes = (ctypes.c_char_p, FloatArray10, ctypes.c_int32, ctypes.c_int32)

# 全個体の標準偏差
# 第四引数に圧縮フラグを設定
getFamilySD = share02_ubu.getFamilySDPy
getFamilySD.restype = ctypes.c_int32
getFamilySD.argtypes = (ctypes.c_char_p, FloatArray10, ctypes.c_int32, ctypes.c_int32)

# あるファイルのトップを長さ64に並び替えた配列で取得
getTopSprm = share02_ubu.getTopSprmPy
getTopSprm.restype = ctypes.c_int32
getTopSprm.argtypes = (ctypes.c_char_p, FloatArray64)

# あるファイルの先頭要素をランダムAIと対戦させ, その結果を取得
getTopSprmGameRsltVSRand = share02_ubu.getTopSprmGameRsltVSRandPy
getTopSprmGameRsltVSRand.restype = ctypes.c_int32
getTopSprmGameRsltVSRand.argtypes = (ctypes.c_char_p, ctypes.c_int32, ctypes.c_int32, ctypes.c_int32, ctypes.c_int32, IntArray3)

# 適応度取得関数
getFitness = share02_ubu.getFitnessPy
getFitness.restype = ctypes.c_int32
getFitness.argtypes = (ctypes.c_char_p, IntArray100, ctypes.c_int32)

# ファイル名決定関数 (引数の数がエグい)
makeSprmFileNameRankGeoProg = share02_ubu.makeSprmFileNameRankGeoProgPy
makeSprmFileNameRankGeoProg.restype = ctypes.c_int32
makeSprmFileNameRankGeoProg.argtypes = (CharArray4096, ctypes.c_int32, ctypes.c_int32, ctypes.c_int32, ctypes.c_int32, ctypes.c_int32, ctypes.c_int32, ctypes.c_double, ctypes.c_int32, ctypes.c_int32)

# 使う Sprm を設定
setSPRM_EFF = share02_ubu.setSPRM_EFFPy
setSPRM_EFF.restype = ctypes.c_int32
setSPRM_EFF.argtypes = (ctypes.c_char_p, ctypes.c_int32, ctypes.c_int32, ctypes.c_int32)

# 使う Sprm を見る
showSPRM_EFF = share02_ubu.showSPRM_EFFPy
showSPRM_EFF.restype = None
showSPRM_EFF.argtypes = None

# 使う Sprm を長さ64の配列で取得
getSPRM_EFF = share02_ubu.getSPRM_EFFPy
getSPRM_EFF.restype = None
getSPRM_EFF.argtypes = (FloatArray64,)

# Sprm を使った αβ で最善手を取得
getBestActABSprm = share02_ubu.getBestActABSprmPy
getBestActABSprm.restype = ctypes.c_int32
getBestActABSprm.argtypes = (IntArray64, ctypes.c_int32, ctypes.c_int32)

# 公比同士対戦関数
# 公比の数は100を超えないこと前提
vsOtherCommonRatio = compare01_share_ubu.vsOtherCommonRatioPy
vsOtherCommonRatio.restype = ctypes.c_int32
vsOtherCommonRatio.argtypes = (IntArray100, ctypes.c_int32, ctypes.c_int32, ctypes.c_int32, ctypes.c_int32, ctypes.c_int32, ctypes.c_int32, ctypes.c_int32, ctypes.c_int32, ctypes.c_uint32, ctypes.c_uint32, ctypes.c_int32)

# ラッパー関数の集まり
class ShareLibWrap():

    def __init__(self):
        pass

    # 各種ラッパー関数
    # n は個体数を指定 (過小はいいがオーバーに注意)
    # 圧縮フラグを追加
    def getFamilyMeansWrap(self, fnamer, n, compressed):
        f_arr_c = FloatArray10()
        flag = getFamilyMeans(fnamer.encode(), f_arr_c, n, compressed)
        res_list = []
        # 読み込み成功
        if flag >= 0:
            res_list = list(f_arr_c)
        return res_list

    # 圧縮フラグを追加
    def getFamilySDWrap(self, fnamer, n, compressed):
        f_arr_c = FloatArray10()
        flag = getFamilySD(fnamer.encode(), f_arr_c, n, compressed)
        if flag < 0:
            return []
        return list(f_arr_c)

    # トップパラメータを長さ64のリストで取得
    def getTopSprmWrap(self, fnamer):
        f_arr_c = FloatArray64()
        if getTopSprm(fnamer.encode(), f_arr_c) < 0:
            return []
        return list(f_arr_c)

    # 適応度取得
    def getFitnessWrap(self, fnamef, n):
        # int配列, 余裕を持って多めに確保
        i_arr_c = IntArray100()
        res_list = []
        # エラーじゃなければ必要なサイズにスライスして戻り値とする
        if getFitness(fnamef.encode(), i_arr_c, n) >= 0:
            res_list = list(i_arr_c)[:n]
        return res_list

    # あるファイルの先頭要素とランダムAIとの試合結果を取得
    # [勝ち数, 引き分け数, 負け数] の順のリストを返す
    # エラーなら [0, 0, 0]
    # 指し手決定関数も指定するように変更
    # 0: 固定, 1: ルーレット
    def getTopSprmGameRsltVSRandWrap(self, fnamer, color, loc_pop, decNxt_id, game_num):
        # 戻り値保存用
        i_arr_c = IntArray3()
        if getTopSprmGameRsltVSRand(fnamer.encode(), color, loc_pop, game_num, decNxt_id, i_arr_c) < 0:
            return [0, 0, 0]
        return list(i_arr_c)

    # ファイル名作成
    # 個体数, エリート数, 選択ID, 公比, 世代数を与える
    # 選択ID: 2: 公比そのまま, 3: 公比自然対数
    # 交叉ID: 5: 2人っ子一様交叉, 6: BLX-α 交叉 (α = 0.3)
    # デフォルトオプションは正規化 & 初期世代ランダム
    def makeSprmFileNameRankGeoProgWrap(self, loc_pop, loc_eln, sel_id, crs_id, loc_seed, cmn_ratio, gene_num, options=0b10):
        c_arr_c = CharArray4096()
        res_str = ""
        str_len = makeSprmFileNameRankGeoProg(c_arr_c, FILENAME_MAX, loc_pop, loc_eln, sel_id, crs_id, loc_seed, cmn_ratio, gene_num, options)
        # 作成に成功したら, バイト型に復号, 長さも調整
        if str_len > 0:
            res_str = bytes(c_arr_c[:str_len]).decode()
        return res_str
    
    # 使う Sprm を設定 (ファイル名をバイト型にして渡すだけ)
    def setSPRM_EFFWrap(self, fnamer, compressed, loc_pop, rank):
        return setSPRM_EFF(fnamer.encode(), compressed, loc_pop, rank)

    # 使う Sprm を長さ64のリストで返す
    def getSPRM_EFFWrap(self):
        f_arr_c = FloatArray64()
        getSPRM_EFF(f_arr_c)
        return list(f_arr_c)
    
    # Sprm を使った αβ で指し手を取得
    def getBestActABSprmWrap(self, board_info, depth, turn):
        board_list = []
        # 番兵を排除して要素を64にする
        for i in range(9, 81, 9):
            for j in range(1, 9):
                board_list.append(board_info[i + j])
        # スターで分解して C の整数配列に変換
        # C で定義しているアドレスで指し手取得
        c_sub = getBestActABSprm(IntArray64(*board_list), depth, turn)
        # 文字列に変換
        # ord("h") = 104, ord("8") = 56
        return chr(104 - c_sub % 16 // 2) + chr(56 - c_sub // 16)
    
    # 公比同士対戦ラッパー関数
    def vsOtherCommonRatioWrap(self, loc_pop, loc_eln, loc_seed, start_cr_th, stop_cr_th, step_cr_th, gene_num, vs_seed, loop, dnfunc_id, zako=0):
        i_arr_c = IntArray100()
        # 対象の公比の数を計算. 適切に与えられていれば割り切れるはず.
        # 植木算? で +1
        rep_pop = (stop_cr_th - start_cr_th) // step_cr_th + 1
        # 対戦
        vsOtherCommonRatio(i_arr_c, loc_pop, loc_eln, start_cr_th, step_cr_th, rep_pop, gene_num, dnfunc_id, loop, loc_seed, vs_seed, zako)
        return list(i_arr_c)[:rep_pop]

if __name__ == "__main__":
    print("Hello!")
    slw = ShareLibWrap()
    #print(slw.makeSprmFileNameRankGeoProgWrap(50, 1, 3, 6, 124, 0.001, 10))
    #fname = slw.makeSprmFileNameRankGeoProgWrap(50, 0, 3, 5, 123, -0.4, 100, options=0b01)
    #print(slw.vsOtherCommonRatioWrap(50, 0, 555, -2000, 2000, 100, 100, 124, 10, 1, 0))
    print(slw.vsOtherCommonRatioWrap(50, 0, 555, -2000, 0, 50, 100, 124, 10, 1, 1))