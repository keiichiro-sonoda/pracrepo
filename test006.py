import sys
import time
import random as rd
import ctypes
import numpy as np
# インポート個別に
# 予約語被る可能性低そうだし, 全部インポートしちゃおうか
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *
import pyqtgraph as pg

# 共有ライブラリ読み込み
# このライブラリがある場所に移動して実行する
exe2_win = ctypes.cdll.LoadLibrary("exe2_win.so")
share01 = ctypes.cdll.LoadLibrary("share01.so")
# c_float が64個の配列型を定義
FloatArray64 = ctypes.c_float * 64
IntArray64 = ctypes.c_int32 * 64
# 共有ライブラリを使う際の初期化
# これが無いと不具合の可能性大
exe2_win.initPy()

# デバッグ関数
#share01.sortTest()

# 関数取得 & 返り値, 引数指定
getTopSprm = exe2_win.getTopSprmPy
getTopSprm.rectype = None
getTopSprm.argtypes = (ctypes.c_char_p, FloatArray64)

getActPrm1L = share01.getActPrm1LPy
getActPrm1L.rectype = ctypes.c_int32
getActPrm1L.argtypes = (IntArray64, ctypes.c_int32)

# ライブラリの関数を使いやすく包みたい
# 引数には欲しい世代番号を与える
# ファイル名で取り出す
def getTopSprmWrap(fname):
    f_arr_c = FloatArray64()
    getTopSprm(fname.encode(), f_arr_c)
    return list(f_arr_c)

class Widget(QWidget):
    # よく使う色は先に定義してみる?
    MYGREEN = QColor(0, 200, 51)
    # 候補手用緑
    CANDGREEN = QColor(124, 252, 0)
    # うっすら緑
    BYAKUROKU = QColor(218, 234, 208)
    # マスの大きさ
    SQLEN = 80
    # グラフ用に12色決めておく
    MYCOLORS = (QColor(0xde, 0x96, 0x10), QColor(0xc9, 0x3a, 0x40),
                QColor(0xff, 0xf0, 0x01), QColor(0xd0, 0x6d, 0x8c),
                QColor(0x65, 0xac, 0xe4), QColor(0xa0, 0xc2, 0x38),
                QColor(0x56, 0xa7, 0x64), QColor(0xd1, 0x6b, 0x16),
                QColor(0xcc, 0x52, 0x8b), QColor(0x94, 0x60, 0xa0),
                QColor(0xf2, 0xcf, 0x01), QColor(0x00, 0x74, 0xbf))
    # 色数値を色に変換
    NUM2COLOR = {1: Qt.black, 2: Qt.white}
    # 8方向を示す数値のタプル
    DIRECTIONS = (-10, -9, -8, -1, 1, 8, 9, 10)
    
    def __init__(self, parent=None):
        super(Widget, self).__init__(parent)
        self.setWindowTitle("Othello")
        # ウィンドウの位置と大きさ
        # サイズはマスから計算
        self.setGeometry(100, 100, self.SQLEN * 11 + 500, self.SQLEN * 11)
        # 背景カラー(微妙にグレー)
        #self.setStyleSheet("background:#eeeeee")
        self.setStyleSheet("background:#ffffff")
        # タグからマスの中心座標に変換したい
        # 需要が出てきそうなのでマスの左上の座標も追加する
        # (マスの中心, マスの左上)
        self.tag2pos = dict()
        # 盤面画像作成
        self.img = QImage(self.SQLEN * 10, self.SQLEN * 10, QImage.Format_ARGB32)
        # 余白幅
        self.margin = self.SQLEN
        # コマ半径
        self.radius = int(self.SQLEN * 0.45)
        # 黒の手番からスタート
        self.turn = 1
        # パスカウンタ(2になったら終了)
        self.pass_count = 0
        # ゲーム終了フラグ
        # まだ使い道無いからスタートフラグも兼ねるか
        self.end_flag = True
        # キャンバスのマウスクリック動作のロックフラグ
        # AIのターンで勝手に操作出来ないようにする予定
        self.press_lock = False
        # プレイヤーがAIか人か判別するための変数
        # players[0] が先手, [1]が後手
        # False が人, True がAI
        self.players = [False, False]
        # ペインター作成?
        imgcanvas = QPainter(self.img)
        # フォント設定
        font = QFont()
        font.setPointSize(15)
        imgcanvas.setFont(font)
        # 座標を示す文字を描画, タグと座標の対応辞書の作成
        for i in range(8):
            # 左側から埋めていく
            x = self.margin + self.SQLEN * i
            # アルファベット表示(列)
            imgcanvas.drawText(x + self.SQLEN // 2 - 10, self.margin - 20, chr(i + 97))
            # 数字表示(行)
            # 変数 x を y 座標の決定に使う(ややこしい)
            imgcanvas.drawText(self.margin - 30, x + self.SQLEN // 2 + 10, chr(i + 49))
            for j in range(8):
                # 正方形を描く
                y = self.margin + self.SQLEN * j
                tag = chr(i + 97) + chr(j + 49)
                # 正方形の中央の座標と左上の座標を記録
                # 左上の座標はタプルのまま!
                self.tag2pos[tag] = (QPoint(x + self.SQLEN // 2, y + self.SQLEN // 2), (x, y))
        
        # 初期盤面設定
        self.setInitBoard(imgcanvas)
        # 評価値リストを入手
        # ファイル名指定
        # クラス内変数で所持
        #self.use_sprm = getTopSprmWrap("prm//simple_prm100.bin")
        #self.use_sprm = getTopSprmWrap("prm//sprm_not_mutate020.bin")
        #self.use_sprm = getTopSprmWrap("prm//sprm_corner0.5_100.bin")
        #self.use_sprm = getTopSprmWrap("prm//sprm_vsrand100.bin")
        #self.use_sprm = getTopSprmWrap("prm//sprm_corner0.5neg_100.bin")
        self.use_sprm = getTopSprmWrap("prm//sprm050_06_rlt_1p_rd005//sprm050_06_rlt_1p_rd005_g100.bin")
        # ボタン等設定
        self.setButtons()
        self.setRadioButtons()
        # タイマー設定
        self.setTimers()
        # 画面切り替えフラグ?
        self.test_flag = False
        # グラフセット
        self.setGraphs()
        # テスト画像初期化
        self.setTestImage()
        #self.setEssImage()
    
    # 初期盤面設定
    # リセットしたいキャンバスを与える
    def setInitBoard(self, imgcanvas):
        # 盤面情報一次元配列を初期化
        # 黒1, 白2, 空0, 番兵-1とする
        self.board_info = [-(i <= 8 or i % 9 == 0 or i >= 81) for i in range(91)]
        # ペン設定
        pen = QPen(Qt.black)
        pen.setWidth(4)
        imgcanvas.setPen(pen)
        # ブラシ設定
        imgcanvas.setBrush(self.MYGREEN)
        # タグと座標の辞書から座標を取り出す
        for pos in self.tag2pos.values():
            imgcanvas.drawRect(*pos[1], self.SQLEN, self.SQLEN)
        
        # 最初の4つのコマを配置
        for tag, c_num in [("d4", 1), ("d5", 2), ("e4", 2), ("e5", 1)]:
            self.putKoma(tag, c_num, imgcanvas)
        # ターンを先手にする
        self.turn = 1
        # 候補手探し
        self.getCandidates()
        # リセット時はエンド状態にしておく
        self.end_flag = True

    # 盤面情報を標準出力で確認する
    # 配列の先頭が右下になるように文字列を組み立てる
    def printBoard(self):
        self._printBoard(self.board_info)
    
    def _printBoard(self, board):
        moji = ""
        # 縦
        for y in range(9, 81, 9):
            # 文字列の先頭を改行文字にする
            moji = "\n" + moji
            # 横
            for x in range(1, 9):
                # 文字列の先頭に加えてゆく
                moji = "{:1d} ".format(board[x + y]) + moji
        print(moji)
    
    # ボタン作成
    def setButtons(self):
        self.test_button = QPushButton("test", self)
        self.test_button.move(100, 820)
        self.test_button.setStyleSheet("\
            font-size:20pt;\
            font-weight:bold;\
            font-family:Monotype Corsiva;\
            background:#ffffff")
        self.test_button.resize(140, 50)
        # クリックされたときに実行
        self.test_button.clicked.connect(self.testClicked)

        # リセットボタン
        self.reset_button = QPushButton("Reset", self)
        self.reset_button.move(100, 750)
        self.reset_button.setStyleSheet("\
            font-size:20pt;\
            font-weight:thin;\
            font-family:Times New Roman;\
            background:#ffffff")
        self.reset_button.resize(140, 50)
        # クリックされたときに実行
        self.reset_button.clicked.connect(self.resetClicked)

        # フォントファミリー, サイズ, 太さ, 斜体かどうかを決定
        font = QFont("Times New Roman", 20, 500, True)
        # スタートボタン
        # プレイヤーの決定等を先に行ない, これを押したらスタートするようにしたい
        self.start_button = QPushButton("START", self)
        self.start_button.setFont(font)
        self.start_button.resize(140, 50)
        self.start_button.move(250, 820)
        self.start_button.clicked.connect(self.startClicked)
    
    # グラフ作成
    # 試合経過と評価値の推移のグラフにしたい
    def setGraphs(self):
        pg.setConfigOptions(
            antialias=True, foreground='k', background=(255, 255, 255)
        )
        self.win = pg.GraphicsWindow(
            size=(500, 400), border=True, parent=self
        )
        self.win.move(800, 50)
        # ローカル変数でも維持されるっぽい?
        # グラフの追加とタイトルの設定
        self.graph = self.win.addPlot(title="Data")
        # 'bottom' は縦軸, 'left' は横軸
        # 'units' は軸の単位
        self.graph.setLabel('left', "point")
        self.graph.setLabel('bottom', "progress")
        # 幅
        self.x_range = [0, 0]
        self.y_range = [-10, 10]
        # 横軸の最小値, 最大値, 縦軸の最小値, 最大値
        # オートスケールにした方が都合が良いかも
        #self.graph.setRange(xRange=self.x_range, yRange=self.y_range)
        #xaxis = self.graph.getAxis('bottom')
        # 横軸の目盛の場所とラベル (数値, ラベル) のタプルのリスト?
        # 数値 = ラベルとしておく
        #x_ticks = [(i, i) for i in range(x_range[0], x_range[1] + 1, 100)]
        #xaxis.setTicks([x_ticks])
        #yaxis = self.graph.getAxis('left')
        # 縦軸の目盛の場所とラベル
        #y_ticks = [(i, i) for i in [-0.5, -0.25, 0, 0.25, 0.5]]
        #yaxis.setTicks([y_ticks])
        # グリッド線の表示
        self.graph.showGrid(x=True, y=True)
        # 反例を表示
        #graph.addLegend()
        # どれだけターンが進行したか初期はターン0
        # 10 マス分のデータの配列を用意
        # 空リストの掛け算同じアドレスが10個コピーされてしまうみたい
        # タプルのリストで試してみる?
        gpen = pg.mkPen((0, 0, 255), width=2)
        # 曲線オブジェクト?
        # これでデータ書き換える
        # このときはデータ与えなくてもいいらしい
        self.curve = self.graph.plot(pen=gpen, name="point")
        # データ初期化
        self.setInitGraph()
    
    # グラフ初期化関数
    def setInitGraph(self):
        # 進行ターン数
        self.progress = 0
        # 横軸リスト
        self.progress_list = [0]
        # 縦軸評価値
        self.points = [0]
        # グラフにデータセット
        self.curve.setData(self.progress_list, self.points)
    
    # ラジオボタンの設定
    def setRadioButtons(self):
        # ラジオボタンのx座標
        x = 800
        self.label1 = QLabel("black", self)
        # テキストの位置を指定
        self.label1.move(x, 470)
        # テキストの詳細設定
        self.label1.setStyleSheet("\
            font-size:10pt;\
            font-weight:500;\
            color:#000000;\
            ")
        # 箱の大きさ変更?
        self.label1.resize(150, 30)
        # 左揃え
        self.label1.setAlignment(Qt.AlignLeft)
        # 先手をAIにするか人間にするか決めるラジオボタン
        self.rgroup1 = QButtonGroup(self)
        self.rbutton1 = QRadioButton("human", self)
        self.rgroup1.addButton(self.rbutton1)
        self.rbutton2 = QRadioButton("AI", self)
        self.rgroup1.addButton(self.rbutton2)
        self.rbutton1.move(x, 500)
        self.rbutton2.move(x, 530)
        self.rbutton1.resize(150, 30)
        self.rbutton2.resize(150, 40)
        # 初期値は人
        self.rbutton1.setChecked(True)
        # プレイヤー変更は片方のラジオボタンだけ見れば大丈夫
        # そもそもラジオボタンよりチェックボックスの方がよかったのでは?
        # それは後々考えよう
        self.rbutton1.toggled.connect(lambda : self.detPlayer(0))

        # フォントの決め方を変える
        font = QFont()
        font.setPointSize(10)
        font.setBold(True)
        self.label2 = QLabel("white", self)
        self.label2.setFont(font)
        self.label2.move(x, 600)
        # 後手をAIにするか人間にするか決めるラジオボタン
        self.rgroup2 = QButtonGroup(self)
        self.rbutton3 = QRadioButton("human", self)
        self.rgroup2.addButton(self.rbutton3)
        self.rbutton4 = QRadioButton("AI", self)
        self.rgroup2.addButton(self.rbutton4)
        self.rbutton3.move(x, 630)
        self.rbutton4.move(x, 660)
        self.rbutton3.resize(150, 30)
        self.rbutton4.resize(150, 30)
        # 初期値は人
        self.rbutton3.setChecked(True)
        # 無名関数?
        self.rbutton3.toggled.connect(lambda : self.detPlayer(1))
    
    # タイマー設定関数
    # 複数形だけど今のところひとつ
    def setTimers(self):
        self.timer = QTimer(self)
        # 一発ずつ実行
        self.timer.setSingleShot(True)
        # 関数を繋げる
        self.timer.timeout.connect(self.randomAction)
        # 待ち時間(ミリ秒)
        self.wait_time = 1000
    
    # ラジオボタンが変更されたとき実行
    def detPlayer(self, index):
        # ブールそのまま代入すればいいのでは?
        if index == 0:
            self.players[0] = self.rbutton2.isChecked()
        else:
            self.players[1] = self.rbutton4.isChecked()
        #print(self.players)
    
    # テスト用画像を作成
    def setTestImage(self):
        self.test_img = QImage(self.SQLEN * 10, self.SQLEN * 10, QImage.Format_ARGB32)
        imgcanvas = QPainter(self.test_img)
        # ペン設定
        pen = QPen()
        pen.setColor(Qt.black)
        pen.setWidth(4)
        imgcanvas.setPen(pen)
        # フォント設定
        font = QFont()
        font.setPointSize(12)
        imgcanvas.setFont(font)
        # なんとなく色配列作っておく
        rgb = [0, 0, 0]
        # 評価値によって色を変えた正方形を描きたい
        for i in range(8):
            for j in range(8):
                # パラメータの添え字を計算
                index = i * 8 + j
                # 評価値を取り出す
                value = self.use_sprm[index]
                # 正なら緑に近づけたい, 負なら赤に
                # 0は黄色
                if value >= 0:
                    rgb[0] = 255 - int(510 * value)
                    rgb[1] = 255
                else:
                    rgb[0] = 255
                    rgb[1] = 255 + int(510 * value)
                # ここで配列分割 * の出番?
                color = QColor(*rgb)
                imgcanvas.setBrush(color)
                # 正方形を描く
                x = self.margin + self.SQLEN * i
                y = self.margin + self.SQLEN * j
                imgcanvas.drawRect(x, y, self.SQLEN, self.SQLEN)
                # 数値も表示
                imgcanvas.drawText(x + 18, y + 45, "{:5.2f}".format(value))
    
    # 主要マスのみ強調した画像を作りたい
    def setEssImage(self):
        self.test_img = QImage(self.SQLEN * 10, self.SQLEN * 10, QImage.Format_ARGB32)
        imgcanvas = QPainter(self.test_img)
        # ペン設定
        pen = QPen()
        pen.setColor(Qt.black)
        pen.setWidth(4)
        imgcanvas.setPen(pen)
        # フォント設定
        font = QFont()
        font.setPointSize(25)
        imgcanvas.setFont(font)
        # 左上の主要マス
        essence = {
            "a1": " 1", "b1": " 2", "b2": " 5", "c1": " 3", "c2": " 6",
            "c3": " 8", "d1": " 4", "d2": " 7", "d3": " 9", "d4": "10"
        }
        for tag, pos in self.tag2pos.items():
            # アンパック代入?
            x, y = pos[1]
            # 主要マスかどうかで色分け
            if tag in essence:
                imgcanvas.setBrush(self.MYGREEN)
                moji = essence[tag]
            else:
                imgcanvas.setBrush(self.BYAKUROKU)
                moji = ""
            imgcanvas.drawRect(x, y, self.SQLEN, self.SQLEN)
            imgcanvas.drawText(x + 15, y + 60, moji)

    # テストボタンが押された
    # 状態の切り替えのみ行なう
    def testClicked(self):
        # 通常に戻す
        if self.test_flag:
            self.test_flag = False
            self.update()
            return
        # テストモードにする
        self.test_flag = True
        self.update()
    
    # ペイント時にはここを実行しなければならない
    def paintEvent(self, event):
        # テスト時
        if self.test_flag:
            canvas = QPainter(self)
            canvas.drawImage(0, 0, self.test_img)
            return
        canvas = QPainter(self)
        canvas.drawImage(0, 0, self.img)
    
    # 押された座標取得
    def mousePressEvent(self, event):
        # ゲーム中以外
        if self.end_flag or self.test_flag:
            return
        if self.press_lock:
            print("locked")
            return
        # 座標をタグに変換
        tag = self.pos2tag(event.pos())
        # 候補手に含まれないなら何もしない
        if tag not in self.candidates:
            return
        # 有効な手なら処理が終わるまでロック
        self.press_lock = True
        # 盤面更新
        self.updateBoard(tag)
        # 描画
        self.update()
        # AIのターンならランダムに打たせる
        # 待ち時間は適当
        if self.players[0] and self.turn == 1:
            self.timer.start(self.wait_time)
        elif self.players[1] and self.turn == 2:
            self.timer.start(self.wait_time)
        # 相手も人, もしくはパスでもう一度指せる場合はロック解除
        else:
            self.press_lock = False
    
    # マウスが動いた時の座標取得?
    def mouseMoveEvent(self, event):
        pass
    
    # マウスが離された
    def mouseReleaseEvent(self, event):
        pass
    
    # QtCore.QPoint のオブジェクトを与えると, 該当するタグを返す
    def pos2tag(self, pos):
        nx = (pos.x() - self.margin) // self.SQLEN
        if (nx < 0 or 7 < nx):
            return "z0"
        ny = (pos.y() - self.margin) // self.SQLEN
        if (ny < 0 or 7 < ny):
            return "z0"
        # ASCII で元に戻す
        return chr(nx + 97) + chr(ny + 49)
    
    # タグから添え字に変換する
    def tag2sub(self, tag):
        return (57 - ord(tag[1])) * 9 + 105 - ord(tag[0])
    
    # 添え字からタグに変換する
    def sub2tag(self, sub):
        return chr(105 - sub % 9) + chr(57 - sub // 9)
    
    # 該当するタグのマスに円を描く
    # 色は数値で指定するように変更
    # 引数にキャンバスを与えることにする
    # キャンバスだけでなく, 配列も同期するようにする
    def putKoma(self, tag, c_num, imgcanvas):
        # 色を得る
        color = self.NUM2COLOR[c_num]
        # 枠も中身も同じ色で統一
        imgcanvas.setPen(color)
        imgcanvas.setBrush(color)
        # 円を描く
        imgcanvas.drawEllipse(self.tag2pos[tag][0], self.radius, self.radius)
        # 配列も書き換え
        self.board_info[self.tag2sub(tag)] = c_num
    
    # リセットボタンクリック時動作
    def resetClicked(self):
        # ポップアップ表示を追加
        reply = QMessageBox.question(self, "Menu", "Do you reset the board?",
            QMessageBox.Yes | QMessageBox.No,
            QMessageBox.Yes
        )
        # Yesが押された場合
        if reply == QMessageBox.Yes:
            print("reset!!")
            # ラジオボタンを有効に戻す
            self.rbutton1.setEnabled(True)
            self.rbutton2.setEnabled(True)
            self.rbutton3.setEnabled(True)
            self.rbutton4.setEnabled(True)
            # スタートボタンも有効に
            self.start_button.setEnabled(True)
            # 盤面を初期状態に
            self.setInitBoard(QPainter(self.img))
            # グラフも初期状態に
            self.setInitGraph()
            # 盤面クリックは有効にしておく
            self.press_lock = False
    
    # スタートボタンクリック時動作
    def startClicked(self):
        print("START!!")
        self.rbutton1.setEnabled(False)
        self.rbutton2.setEnabled(False)
        self.rbutton3.setEnabled(False)
        self.rbutton4.setEnabled(False)
        # ゲーム中はボタン無効化
        self.start_button.setEnabled(False)
        # 候補手色塗り
        self.coloringCandidates(QPainter(self.img))
        # 画像変更を適用
        self.update()
        # ゲーム開始!
        self.end_flag = False
        # 先手がAIの場合
        if self.players[0]:
            # ロック
            self.press_lock = True
            # 1発だけ
            self.timer.start(1)
    
    # 新バージョン?
    # 候補手辞書を作成する以外に, 終了処理も行なう
    def getCandidates(self):
        # 候補手辞書を取得
        cand_local = self.getCandidatesLocal(self.board_info, self.turn)
        # 空辞書の場合
        if not cand_local:
            print("パス")
            # ターン変更して探索し直す
            self.turn ^= 3
            cand_local = self.getCandidatesLocal(self.board_info, self.turn)
            # さらに空辞書の場合, 終了処理
            if not cand_local:
                self.end_flag = True
                self.resultPopup()
        # 候補手辞書をクラス内変数に代入
        self.candidates = cand_local

    # 候補手のところの色を変える
    # 元に戻すなら back を True に
    def coloringCandidates(self, imgcanvas, back=False):
        # ペン設定
        pen = QPen(Qt.black)
        pen.setWidth(4)
        imgcanvas.setPen(pen)
        # ブラシ設定
        if back:
            imgcanvas.setBrush(self.MYGREEN)
        else:
            imgcanvas.setBrush(self.CANDGREEN)
        for tag in self.candidates.keys():
            # 正方形の左上の座標(タプル)を取得してスターで分解
            imgcanvas.drawRect(*self.tag2pos[tag][1], self.SQLEN, self.SQLEN)

    # 盤面更新
    # かなり重要な関数だと思う
    # コマを置くタグを渡す
    # 画像を既に読み込んでいる関数から呼び出してはいけない
    def updateBoard(self, tag):
        # キャンバス取り出し
        imgcanvas = QPainter(self.img)
        # 候補手の色を元に戻す
        self.coloringCandidates(imgcanvas, back=True)
        # ひっくり返すマスの添え字を代入
        for sub in self.candidates[tag]:
            # タグに変換してコマをひっくり返す
            self.putKoma(self.sub2tag(sub), self.turn, imgcanvas)
        # コマを置く
        self.putKoma(tag, self.turn, imgcanvas)
        # 盤面情報表示
        self.printBoard()
        # ターンを変更して候補手探し
        # 次がパスならターンが元に戻る可能性あり
        self.turn ^= 3
        self.getCandidates()
        # 次の候補手を色塗り
        self.coloringCandidates(imgcanvas)
        # 進行度インクリメント
        self.progress += 1
        self.progress_list.append(self.progress)
        # 今の盤面の評価値を追加
        self.points.append(self.evaluationBySprm(self.board_info))
        self.curve.setData(self.progress_list, self.points)
        # 描画適用
        self.update()
    
    # 結果のポップアップ
    def resultPopup(self):
        # それぞれのコマを数える
        bc = sum(1 for i in self.board_info if i == 1)
        wc = sum(1 for i in self.board_info if i == 2)
        dif = bc -wc
        if dif == 0:
            moji = "引き分け"
        if dif > 0:
            moji = "黒の{:2d}枚勝ち".format(dif)
        elif dif < 0:
            moji = "白の{:2d}枚勝ち".format(-dif)
        # 選択肢はOKのみ
        reply = QMessageBox.question(self, "対局終了", moji,
            QMessageBox.Ok,
            QMessageBox.Ok
        )
    
    # 候補手からランダムに選択
    # ランダムとか言いつつそれ以外も担当している
    def randomAction(self):
        cand_list = list(self.candidates.keys())
        # 候補手が存在しない
        if not cand_list:
            return
        # ランダムで選んで盤面更新
        #self.updateBoard(rd.choice(cand_list))
        # 試しに呼び出してみる
        self.getActWithCFunc()
        # シンプルパラメータを用いた最善手
        tag = self.getBestActBySprm(self.board_info, self.turn)
        # 盤面更新
        self.updateBoard(tag)
        # 次が人ならターンをロック解除
        if not self.players[0] and self.turn == 1:
            self.press_lock = False
        elif not self.players[1] and self.turn == 2:
            self.press_lock = False
        # 次もAIならすぐにこの関数を実行
        else:
            self.timer.start(1)
    
    # 盤面のシンプル評価
    # 評価値が高い程黒が有利と考える
    def evaluationBySprm(self, board):
        # 使う重みの添え字
        c = 0
        pt = 0
        # 番兵以外の添え字を繰り返す
        # 重みリストの添え字と対応する順番で
        for y in range(9, 81, 9):
            for x in range(1, 9):
                # コマ取得
                koma = board[x + y]
                # 黒なら評価値に重みを足す, 白なら引く
                # 空白は何もしない
                if koma == 1:
                    pt += self.use_sprm[c]
                elif koma == 2:
                    pt -= self.use_sprm[c]
                # 次の重みを参照
                c += 1
        return pt

    # クラス内変数の候補手ディクショナリは書き換えないように候補手を探索
    # 引数には盤面情報リスト、手番(ターン)を与える
    # パス処理はこの関数のラッパー関数で行なう予定
    # やっぱり盤面情報の引継ぎとかめんどいから一関数でまとめてみよう
    def getCandidatesLocal(self, board, teban):
        # 候補初期化
        cand_local = dict()
        # 相手の手番(コマの値)を計算
        opponent = teban ^ 3
        # タグ座標辞書から全てのタグを取り出す
        for tag in self.tag2pos.keys():
            # タグを添え字に変換
            sub = self.tag2sub(tag)
            # 空マスじゃなければやり直し
            if board[sub] != 0:
                continue
            # 全方向のひっくり返す添え字候補
            rev_tags = []
            # 全方向探索
            for d in self.DIRECTIONS:
                # 一方向のひっくり返す添え字候補
                tmp = []
                # 隣のマスのコマをチェック
                next_sub = sub + d
                koma = board[next_sub]
                # 相手のコマの場合, それ以外が出るまで対角マスを探索
                while koma == opponent:
                    # リストに追加
                    tmp.append(next_sub)
                    # 先のマスへ進みコマをチェック
                    next_sub += d
                    koma = board[next_sub]
                # 自分のコマが出たら, これまでの添え字を候補に追加
                # 空のマスか番兵が出たら, その方向のリストは無効化
                # 探索マスの隣が自分のコマなら, 空リストを足すだけ
                if koma == teban:
                    rev_tags += tmp
            # ひっくり返せるマスが存在する場合
            # タグを辞書のキーとし, ひっくり返すマスの添え字リストを値にする
            if rev_tags:
                cand_local[tag] = rev_tags
        # 辞書を返す(空なら空のまま)
        return cand_local
    
    # タグをキーとし, 次の盤面のリストを値とする辞書を作成
    def getNextBoards(self, board, teban):
        # 空辞書で初期化
        next_boards = dict()
        cand_local = self.getCandidatesLocal(board, teban)
        # 候補タグとひっくり返すマスの添え字リストを繰り返し代入
        for c_tag, c_subs in cand_local.items():
            # 盤面コピーを作成
            nb = board.copy()
            # ひっくり返すマスを繰り返し代入
            for sub in c_subs:
                # 自分のコマに書き換え
                nb[sub] = teban
            # コマを置く
            nb[self.tag2sub(c_tag)] = teban
            # 辞書にキーと要素を追加
            next_boards[c_tag] = nb
        # 次の盤面の辞書を返す
        # 候補手が無ければ空辞書
        return next_boards
    
    # パラメータから最善手を得たい
    # 次の手で決着できる(勝てる)としても, 重みを使った評価だけで判断
    def getBestActBySprm(self, board, teban):
        # 次の盤面辞書を得る
        next_boards = self.getNextBoards(board, teban)
        # 返り値となるタグ(初期値は無効文字列)
        best_tag = "z0"
        # 先手の場合
        if teban == 1:
            mx = -float("inf")
            # 指し手と盤面を取り出す
            for tag, nb in next_boards.items():
                pt = self.evaluationBySprm(nb)
                # 暫定最大値なら更新
                if mx < pt:
                    mx = pt
                    best_tag = tag
            print(mx, best_tag)
        # 後手の場合
        else:
            mn = float("inf")
            for tag, nb in next_boards.items():
                pt = self.evaluationBySprm(nb)
                # 暫定最小値なら更新
                if pt < mn:
                    mn = pt
                    best_tag = tag
            print(mn, best_tag)
        # 返り値を間違えていた
        return best_tag
    
    # C言語の共有ライブラリで定義された関数で指し手を決定する
    def getActWithCFunc(self):
        # Cに渡すための盤面リスト
        board_list = []
        # 番兵を除いてリストを作る
        for i in range(9, 81, 9):
            for j in range(1, 9, 1):
                board_list.append(self.board_info[i + j])
        # スターを付けて渡し, cのint型配列にする
        i_arr_c = IntArray64(*board_list)
        getActPrm1L(i_arr_c, self.turn)

class Application(QApplication):
    def __init__(self):
        super(Application, self).__init__(sys.argv)
        # QWidgetの自作子クラス
        self.gui = Widget()
        # ウィンドウの大きさと表示位置を指定する
        # left, top, width, height
        # topは50以下くらいになると画面からはみ出る
        # 自作クラスの初期関数で設定した
        #self.gui.setGeometry(20, 50, 800, 800)
        # ウィンドウの設定
        #self.gui.setStyleSheet("background:#eeeeee")
        # ラベル
        #self.labelTest()
        # 様々なボタン定義
        self.setButtons()

    # テキスト表示
    def labelTest(self):
        moji = "PyQtを勉強中\n園田継一郎 17T2088B"
        self.label = QLabel(moji, self.gui)
        # テキストの位置を指定
        self.label.move(100, 100)
        # テキストの詳細設定
        # CSSフォーマットとは何ぞや
        self.label.setStyleSheet("\
            font-size:20pt;\
            font-weight:bold;\
            color:#ff00ff;\
            background-color:#00ff00;\
            ")
        # 箱の大きさ変更?
        self.label.resize(400, 100)
        # 中央揃え
        self.label.setAlignment(Qt.AlignCenter)
    
    def setButtons(self):
        y = 750
        # 終了ボタン
        self.end_button = QPushButton("終了", self.gui)
        self.end_button.move(250, y)
        self.end_button.setStyleSheet("\
            font-size:15pt;\
            font-weight:100;\
            font-family:游明朝;\
            background:#ff0000")
        self.end_button.clicked.connect(self.clickEndButton)
        # 通常表示ボタン
        self.show_normal_button = QPushButton("通常表示", self.gui)
        self.show_normal_button.move(400, y)
        self.show_normal_button.setStyleSheet("\
            font-size:15pt;\
            font-weight:800;\
            font-family:游明朝;\
            background:lime green")
        self.show_normal_button.clicked.connect(self.gui.showNormal)
        # 全画面ボタン
        self.show_full_button = QPushButton("全画面", self.gui)
        self.show_full_button.move(550, y)
        self.show_full_button.setStyleSheet("\
            font-size:10pt;\
            font-weight:600;\
            font-family:游ゴシック;\
            background:#ffff00")
        self.show_full_button.resize(100, 50)
        self.show_full_button.clicked.connect(self.gui.showFullScreen)
    

    # 終了ボタン
    def clickEndButton(self):
        reply = QMessageBox.question(self.gui, "終了", "終了しますか?",
            QMessageBox.Yes | QMessageBox.No | QMessageBox.Cancel,
            QMessageBox.Yes
        )
        if reply == QMessageBox.Yes:
            print("end")
            sys.exit()

    # テキスト変更
    def changeText(self):
        self.label.setText("Text Changes!")
    
    def changeButtonText(self):
        self.button.setText("Changed")

    def run(self):
        self.gui.show()
        sys.exit(self.exec_())


def main():
    app = Application()
    app.run()

if __name__ == "__main__":
    main()