# pyqtgraph を学びたい
# プロットの詳細設定を追加したい
import sys
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *

import pyqtgraph as pg

class GraphWindow(QMainWindow):
    def __init__(self, parent=None):
        super(GraphWindow, self).__init__(parent)

        # 枠線, 軸の方向を設定
        vb = pg.ViewBox(
            border=pg.mkPen(color='#000000'),
            invertX=False,
            invertY=True
        )

        # PlotWidgetを作成する
        pw = pg.PlotWidget(viewBox=vb)

        # ウィンドウにウィジェットを設定する
        self.setCentralWidget(pw)

        # 背景色を設定(透過?)
        pw.setBackground("#ffffff00")

        # グラフのサイズ固定
        pw.setMinimumSize(500, 400)
        pw.setMaximumSize(500, 400)

        # plotItemを呼び出す
        p1 = pw.plotItem


        # データ
        x = [0, 1, 2, 3, 4]
        y1 = x.copy()
        y2 = [4 - i for i in x]

        # 散布図と折れ線グラフを描写する
        curve = pg.PlotCurveItem(
            x=x, y=y1,
            pen=pg.mkPen(color="r", style=Qt.SolidLine),
            antialias=True
        )
        scatter = pg.ScatterPlotItem(
            x=x, y=y2,
            pen=pg.mkPen(None),
            brush=pg.mkBrush('b'),
            size=20.0,
            antialias=True
        )
        p1.addItem(curve)
        p1.addItem(scatter)

if __name__ == '__main__':
    # Qt Application を作ります
    app = QApplication(sys.argv)
    # form を作成して表示します
    mainWin = GraphWindow()
    mainWin.show()
    # Qt のメインループを開始します
    sys.exit(app.exec_())