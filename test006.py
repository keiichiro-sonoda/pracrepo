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
# c_float が64個の配列型を定義
FloatArray64 = ctypes.c_float * 64
# 共有ライブラリを使う際の初期化
# これが無いと不具合の可能性大
exe2_win.initPy()
# 初期盤面確認
#exe2_win.showStartPy()
# 関数取得 & 返り値, 引数指定
getSprmFile = exe2_win.getSprmFilePy
getSprmFile.rectype = None
getSprmFile.argtypes = (ctypes.c_int32, FloatArray64)

# ライブラリの関数を使いやすく包みたい
# 引数には欲しい世代番号を与える
def getSprmFileWrap(n):
    f_arr_c = FloatArray64()
    getSprmFile(n, f_arr_c)
    # リストに戻して返す
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
        self.setStyleSheet("background:#eeeeee")
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
        # タイマー設定
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.randomAction)
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
        # 確認するならここの値を変更
        # クラス内変数で所持
        self.use_sprm = getSprmFileWrap(601)
        # ボタン等設定
        self.setButtons()
        self.setRadioButtons()
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
        # 色塗り..はスタートしてから
        #self.coloringCandidates(imgcanvas)
        # リセット時はエンド状態にしておく
        self.end_flag = True

    # 盤面情報を標準出力で確認する
    # 配列の先頭が右下になるように文字列を組み立てる
    def printBoard(self):
        moji = ""
        # 縦
        for y in range(9, 81, 9):
            # 横
            for x in range(1, 9):
                # 文字列の先頭に加えてゆく
                moji = "{:1d} ".format(self.board_info[x + y]) + moji
            # 横が一通り終わったら改行
            moji = "\n" + moji
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
        graph = self.win.addPlot(title="Data")
        # 'bottom' は縦軸, 'left' は横軸
        # 'units' は軸の単位
        graph.setLabel('left', "point")
        graph.setLabel('bottom', "generation")
        x_range = [0, 300]
        y_range = [-0.5, 0.5]
        # 横軸の最小値, 最大値, 縦軸の最小値, 最大値
        graph.setRange(xRange=x_range, yRange=y_range)
        xaxis = graph.getAxis('bottom')
        # 横軸の目盛の場所とラベル (数値, ラベル) のタプルのリスト?
        # 数値 = ラベルとしておく
        x_ticks = [(i, i) for i in range(x_range[0], x_range[1] + 1, 100)]
        xaxis.setTicks([x_ticks])
        yaxis = graph.getAxis('left')
        # 縦軸の目盛の場所とラベル
        y_ticks = [(i, i) for i in [-0.5, -0.25, 0, 0.25, 0.5]]
        yaxis.setTicks([y_ticks])
        # グリッド線の表示
        graph.showGrid(x=True, y=True)
        # 反例を表示
        graph.addLegend()
        x = []
        # 10 マス分のデータの配列を用意
        # 空リストの掛け算同じアドレスが10個コピーされてしまうみたい
        # タプルのリストで試してみる?
        ys = [[] for i in range(10)]
        essence = (0, 1, 2, 3, 9, 10, 11, 18, 19, 23)
        #print(ys)
        for i in range(x_range[0], x_range[1] + 1):
            x.append(i)
            # i 世代のパラメータを取り出す(等価マスも含む64個)
            tprm = getSprmFileWrap(i)
            #print(tprm[0], tprm[56])
            #print(tprm)
            # 主要な10個の要素だけ取り出す
            for j, k in enumerate(essence):
                ys[j].append(tprm[k])
                #print(ys)

        gpen = pg.mkPen((0, 0, 0), width=2)
        for i in range(10):
            gpen.setColor(self.MYCOLORS[i])
            graph.plot(x, ys[i], pen=gpen, name=i)
    
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
    
    # ラジオボタンが変更されたとき実行
    def detPlayer(self, index):
        # ブールそのまま代入すればいいのでは?
        if index == 0:
            self.players[0] = self.rbutton2.isChecked()
        else:
            self.players[1] = self.rbutton4.isChecked()
        print(self.players)
    
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
        # ゲーム中以外もしくはロック中
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
        # そもそも有効な手の場合のみロックを掛ければいいのでは?
        self.press_lock = True
        # 盤面更新
        self.updateBoard(tag)
        # 描画
        self.update()
        # AIのターンならランダムに打たせる
        if self.players[1] and self.turn == 2:
            self.randomAction()
        if self.players[0] and self.turn == 1:
            self.randomAction()
        # 鍵を外す
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
    # 呼び出し元で既に self.img が使われているとエラーになる
    # 引数にキャンバスを与えることにする
    # キャンバスだけでなく, 配列も同期するようにする
    def putKoma(self, tag, c_num, imgcanvas):
        # 中心座標を得る
        center = self.tag2pos[tag][0]
        # 色を得る
        color = self.NUM2COLOR[c_num]
        # 枠も中身も同じ色で統一
        imgcanvas.setPen(color)
        imgcanvas.setBrush(color)
        # 円を描く
        imgcanvas.drawEllipse(center, self.radius, self.radius)
        # 配列も書き換え
        sub = self.tag2sub(tag)
        self.board_info[sub] = c_num
        # 盤面表示(確認用)
        #self.printBoard()
    
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
        # 両方AIの場合
        if self.players[0] and self.players[1]:
            self.press_lock = True
            # 1発だけ
            self.timer.setSingleShot(True)
            self.timer.start(1)
    
    # 置ける場所を探す
    def getCandidates(self):
        # 候補初期化
        # 置ける場所のタグをキーとし, ひっくり返すマスの
        # 一次元配列の添え字をキーとする
        self.candidates = dict()
        for tag in self.tag2pos.keys():
            # 空マスじゃなければやり直し
            if self.board_info[self.tag2sub(tag)] != 0:
                continue
            # 空マスなら探索
            self.search(tag)
        
        # 候補手がない
        if len(self.candidates) == 0:
            print("パス")
            self.pass_count += 1
            # パス2回連続で終了
            if self.pass_count == 2:
                # エンドフラグを立てる
                self.end_flag = True
                # 結果のポップアップ表示へ
                self.resultPopup()
            # パス1回目
            else:
                # ターン変更してもう一度探索
                self.turn ^= 3
                self.getCandidates()
        # 候補手がある
        else:
            self.pass_count = 0

    # 探索関数
    def search(self, tag):
        sub = self.tag2sub(tag)
        # 8方向探索
        for d in self.DIRECTIONS:
            # リスト初期化
            rev_tags = []
            # その方向でひっくり返せるマスのリストを取得
            # 空リストを参照渡しで書き換えてもらう
            self._search(sub + d, d, rev_tags)
            # ひっくり返せるマスがないなら他の方向を探索
            if not rev_tags:
                continue
            # 既に候補に入っている
            if tag in self.candidates:
                self.candidates[tag] += rev_tags
            # 初めてひっくり返せる方向が見つかった
            else:
                self.candidates[tag] = rev_tags
    
    # 一定方向探索関数(再帰)
    # sub: 現在のマスの添え字
    # d: 探索方向
    # rev_tags: これまででひっくり返せるコマの候補
    def _search(self, sub, d, rev_tags):
        # コマ取得
        koma = self.board_info[sub]
        # 空マスか番兵の場合, ひっくり返せるマスは無い
        if koma <= 0:
            rev_tags.clear()
            return
        # 自分のコマの場合
        if koma == self.turn:
            # これまでのリストを返す
            return
        # 相手のコマの場合
        else:
            # ひっくり返せるマスに追加し, 先のマスを探索
            rev_tags.append(sub)
            self._search(sub + d, d, rev_tags)
    
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
        self.turn ^= 3
        self.getCandidates()
        # 次の候補手を色塗り
        self.coloringCandidates(imgcanvas)
    
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
    def randomAction(self):
        cand_list = list(self.candidates.keys())
        # 候補手が存在しない
        if not cand_list:
            return
        self.updateBoard(rd.choice(cand_list))
        self.update()
        # お互いAIならすぐに次を実行
        if self.players[0] and self.players[1]:
            self.timer.start(1)

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