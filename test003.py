# pyqtgraph を学びたい
import sys
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *

import pyqtgraph as pg

class GraphWindow(QMainWindow):
    def __init__(self, parent=None):
        super(GraphWindow, self).__init__(parent)

        # PlotWidgetを作成する
        pw = pg.PlotWidget()

        # ウィンドウにウィジェットを設定する
        self.setCentralWidget(pw)

        # plotItemを呼び出す
        p1 = pw.plotItem

        # データ
        x = [0, 1, 2, 3, 4]
        y1 = x.copy()
        y2 = [4 - i for i in x]

        # 散布図と折れ線グラフを描写する
        p1.addItem(pg.PlotCurveItem(x=x, y=y1))
        p1.addItem(pg.ScatterPlotItem(x=x, y=y2))

if __name__ == '__main__':
    # Qt Application を作ります
    app = QApplication(sys.argv)
    # form を作成して表示します
    mainWin = GraphWindow()
    mainWin.show()
    # Qt のメインループを開始します
    sys.exit(app.exec_())