import sys
import time
import random as rd
import ctypes
# インポート個別に
# 予約語被る可能性低そうだし, 全部インポートしちゃおうか
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *

# 共有ライブラリ読み込み
exe2_win = ctypes.cdll.LoadLibrary(".//exe2_win.so")
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

class Widget(QWidget):
    # よく使う色は先に定義してみる?
    MYGREEN = QColor(0, 200, 51)
    # マスの大きさ
    SQLEN = 80
    # ペイントみたいに線をマウスで線を描いてみる?
    def __init__(self, parent=None):
        super(Widget, self).__init__(parent)
        self.setWindowTitle("Othello")
        # ウィンドウの位置と大きさ
        # サイズはマスから計算
        self.setGeometry(100, 100, self.SQLEN * 11, self.SQLEN * 11)
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
        # オセロ盤面設定
        imgcanvas = QPainter(self.img)
        pen = QPen(Qt.black)
        pen.setWidth(4)
        imgcanvas.setPen(pen)
        imgcanvas.setBrush(self.MYGREEN)
        font = QFont()
        font.setPointSize(15)
        imgcanvas.setFont(font)
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
        
        # float[64]のインスタンスを作成
        f_arr_c = FloatArray64()
        # 300世代目のトップ評価値を入手
        getSprmFile(300, f_arr_c)
        # タプルとしてクラス内変数とする
        self.use_sprms = tuple(f_arr_c)
        # チェック
        #print(self.use_sprms)
        self.setButtons()
    
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
        #self.test_button.clicked.connect(self.clickButton)
    
    # 関数をオーバーライド?
    # print関数入れると, この関数の実行頻度半端ねぇ
    # 重い処理をここでするのは不向きらしい
    def paintEvent(self, event):
        # ここで休止を入れるとかくかくになる
        canvas = QPainter(self)
        canvas.drawImage(0, 0, self.img)
    
    # 押された座標取得
    def mousePressEvent(self, event):
        #print(type(event.pos()))
        # 座標をタグに変換
        tag = self.pos2tag(event.pos())
        # チェック
        print(tag)
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