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
    # マスの大きさ
    SQLEN = 80
    # グラフ用に12色決めておく
    MYCOLORS = (QColor(0xde, 0x96, 0x10), QColor(0xc9, 0x3a, 0x40),
                QColor(0xff, 0xf0, 0x01), QColor(0xd0, 0x6d, 0x8c),
                QColor(0x65, 0xac, 0xe4), QColor(0xa0, 0xc2, 0x38),
                QColor(0x56, 0xa7, 0x64), QColor(0xd1, 0x6b, 0x16),
                QColor(0xcc, 0x52, 0x8b), QColor(0x94, 0x60, 0xa0),
                QColor(0xf2, 0xcf, 0x01), QColor(0x00, 0x74, 0xbf))
    # ペイントみたいに線をマウスで線を描いてみる?
    def __init__(self, parent=None):
        super(Widget, self).__init__(parent)
        self.setWindowTitle("Othello")
        # ウィンドウの位置と大きさ
        # サイズはマスから計算
        self.setGeometry(100, 100, self.SQLEN * 11 + 500, self.SQLEN * 11)
        # 背景カラー(微妙にグレー)
        self.setStyleSheet("background:#eeeeee")
        # タグからマスの中心座標に変換したい
        self.tag2pos = dict()
        # 初期盤面キャンバス?
        self.img = QImage(self.SQLEN * 10, self.SQLEN * 10, QImage.Format_ARGB32)
        # 余白幅
        self.margin = self.SQLEN
        # コマ半径
        self.radius = int(self.SQLEN * 0.45)
        # 盤面情報1次元配列(番兵付き)
        # 黒1, 白2, 空0, 番兵-1とする
        self.board_info = [int(i <= 8 or i % 9 == 0 or i >= 81) for i in range(91)]
        print(self.board_info)
        # オセロ盤面設定
        imgcanvas = QPainter(self.img)
        pen = QPen(Qt.black)
        pen.setWidth(4)
        imgcanvas.setPen(pen)
        imgcanvas.setBrush(self.MYGREEN)
        font = QFont()
        font.setPointSize(15)
        imgcanvas.setFont(font)
        # 空盤面を作る
        for i in range(8):
            # アルファベット表示(列)
            imgcanvas.drawText(self.margin + self.SQLEN * i + self.SQLEN // 2 - 10, self.margin - 20, chr(i + 97))
            # 数字表示(行)
            imgcanvas.drawText(self.margin - 30, self.margin + self.SQLEN * i + self.SQLEN // 2, chr(i + 49))
            for j in range(8):
                # 正方形を描く
                x = self.margin + self.SQLEN * i
                y = self.margin + self.SQLEN * j
                imgcanvas.drawRect(x, y, self.SQLEN, self.SQLEN)
                tag = chr(i + 97) + chr(j + 49)
                # 正方形の中央の座標を記録
                self.tag2pos[tag] = QPoint(x + self.SQLEN // 2, y + self.SQLEN // 2)
        
        # 初期盤面設定
        self.setInitBoard()
        # 評価値リストを入手
        # 確認するならここの値を変更
        # クラス内変数で所持
        self.use_sprm = getSprmFileWrap(601)
        # チェック
        #print(self.use_sprms)
        self.setButtons()
        # 画面切り替えフラグ?
        self.test_flag = False
        # グラフセット
        self.setGraphs()
    
    # 初期盤面を設定したい
    def setInitBoard(self):
        #imgcanvas = QPainter(self.img)
        pass
    
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
        # テスト画像初期化
        self.setTestImage()
    
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
                imgcanvas.drawText(x + 18, y + 45, "{:5.2f}".format(value))

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
    
    # 関数をオーバーライド?
    # print関数入れると, この関数の実行頻度半端ねぇ
    # 重い処理をここでするのは不向きらしい
    def paintEvent(self, event):
        if self.test_flag:
            canvas = QPainter(self)
            canvas.drawImage(0, 0, self.test_img)
            return
        # ここで休止を入れるとかくかくになる
        canvas = QPainter(self)
        canvas.drawImage(0, 0, self.img)
    
    # 押された座標取得
    def mousePressEvent(self, event):
        #print(type(event.pos()))
        # 座標をタグに変換
        tag = self.pos2tag(event.pos())
        # チェック
        #print(tag)
        # 範囲外
        if tag == "z0":
            return
        # 色をランダム指定
        color = rd.choice([Qt.white, Qt.black])
        # そこにコマを置く
        self.putKoma(tag, color)
        # 描画
        self.update()
    
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
    
    # 該当するタグのマスに円を描く
    # 色も指定(デフォルトは黒)
    def putKoma(self, tag, color=Qt.black):
        # 中心座標を得る
        center = self.tag2pos[tag]
        # イメージを取り出す
        imgcanvas = QPainter(self.img)
        # 枠も中身も同じ色で統一
        imgcanvas.setPen(color)
        imgcanvas.setBrush(color)
        imgcanvas.drawEllipse(center, self.radius, self.radius)

    # マウスが動いた時の座標取得?
    def mouseMoveEvent(self, event):
        pass
    
    # マウスが離された
    def mouseReleaseEvent(self, event):
        pass
    
    # ボタンを押したときのスロット?
    def onButtonClick(self):
        print("yeah")
        # 500 * 500 ピクセルの画像を作成
        #self.img = QImage(500, 500, QImage.Format_ARGB32)
        # 画像の QPainter を取得?
        imgcanvas = QPainter(self.img)
        # imgcanvasに対して色々な描画処理
        # 黒玉を描いてみる?
        # 色指定方法やっとわかった
        a = QColor(0, 0, 255)
        print(a)
        imgcanvas.setPen(a)
        # 塗りつぶす色
        imgcanvas.setBrush(a)
        imgcanvas.drawEllipse(QPoint(100, 100), 100, 100)
        # 型を知りたい
        # PyQt5.Qtcore.Qt.GlobalColor というものらしい
        self.update()

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
        # ボタン
        self.button = QPushButton("Button", self.gui)
        self.button.move(100, y)
        self.button.setStyleSheet("\
            font-size:20pt;\
            font-weight:bold;\
            font-family:Monotype Corsiva;\
            background:#ffffff")
        self.button.resize(140, 50)
        # クリックされたときに実行
        self.button.clicked.connect(self.clickButton)
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
        self.show_full_button.resize(100, 100)
        self.show_full_button.clicked.connect(self.gui.showFullScreen)
    
    # クリック時動作
    def clickButton(self):
        #print("Clicked!!")
        # ポップアップ表示を追加
        reply = QMessageBox.question(self.gui, "Menu Text", "Question",
            QMessageBox.Yes | QMessageBox.No,
            QMessageBox.Yes
        )
        if reply == QMessageBox.Yes:
            print("Yes clicked")
            # 黒い円を描く
            self.gui.onButtonClick()
        self.button.setText("Clicked")

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